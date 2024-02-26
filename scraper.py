import argparse
import os
import re
import requests
import threading

from bs4 import BeautifulSoup

ARTIST_RANGE_STEP = 125
GENIUS_TOKEN = os.environ.get('GENIUS_ACCESS_TOKEN')
GENIUS_API_URL = 'http://api.genius.com'
HEADERS = {'Authorization': f'Bearer {GENIUS_TOKEN}',
           'Content-Type': 'application/json'}
SONGS_LIMIT_PER_REQUEST = 50


def artists_songs_url(artist_id: int, page: int = 1) -> str:
    return '/'.join([GENIUS_API_URL, 'artists', str(artist_id), 'songs']) + f'?per_page={SONGS_LIMIT_PER_REQUEST}&page={page}'


def song_url(song_path: str) -> str:
    return GENIUS_API_URL + song_path


def get_lyrics(song_url: str) -> str:
    resp = requests.get(song_url, headers=HEADERS)
    resp.raise_for_status()

    html = BeautifulSoup(
        resp.text.replace('<br/>', '\n'),
        'html.parser'
    )

    # Determine the class of the div
    divs = html.find_all('div', class_=re.compile(
        '^lyrics$|Lyrics__Container'))
    if divs is None or len(divs) <= 0:
        return None

    lyrics = '\n'.join([div.get_text() for div in divs])

    lyrics = re.sub(r'(\[.*?\])*', '', lyrics)
    lyrics = re.sub('\n{2}', '\n', lyrics)  # Gaps between verses

    return lyrics.strip('\n')


def process_artist(artist_id: int, directory: str) -> None:
    print(f'Processing artist_id={artist_id}')
    next_page = process_single_artist_page(artist_id, directory)

    while next_page is not None:
        print(f'Processing artist_id={artist_id}, page={next_page}')
        next_page = process_single_artist_page(
            artist_id, directory, int(next_page))


def process_single_artist_page(artist_id: int, directory: str, page: int = 1):
    with requests.get(artists_songs_url(artist_id, page), headers=HEADERS) as resp:
        if resp.status_code != 200:
            return None

        for song in resp.json()['response']['songs']:
            if song['primary_artist']['id'] != artist_id or song['lyrics_state'] != 'complete':
                continue

            try:
                lyrics = get_lyrics(song['url'])

                if lyrics is None or len(lyrics) == 0:
                    continue

                with open(os.path.join(directory, song['full_title']), 'w') as f:
                    f.write(lyrics)
            except Exception as e:
                print(e)

        return resp.json()['response']['next_page']


def process_artist_range(begin: int, end: int, directory: str):
    for artist_id in range(begin, end):
        process_artist(artist_id, directory)


parser = argparse.ArgumentParser()
parser.add_argument(
    '--dir', help='Directory to save files with lyrics to', required=True)
args = parser.parse_args()

for i in range(1003, 2004, ARTIST_RANGE_STEP):
    thread = threading.Thread(
        target=process_artist_range, args=(i, i + ARTIST_RANGE_STEP, args.dir))
    thread.start()
