#include "index.h"

#include <iostream>

namespace {
    template<typename T>
    void writeAsBinary(std::ostream& stream, T value) {
        stream.write(reinterpret_cast<const char*>(&value), sizeof(T));
    }

    void writeStringAsBinary(std::ostream& stream, const std::string& str) {
        writeAsBinary(stream, str.size());
        stream.write(str.data(), str.size());
    }

    template<typename T>
    void readFromBinary(std::istream& stream, T& value) {
        stream.read(reinterpret_cast<char*>(&value), sizeof(T));
    }

    void readStringFromBinary(std::istream& stream, std::string& str) {
        uint64_t str_size;
        readFromBinary(stream, str_size);

        str.resize(str_size);

        stream.read(str.data(), str.size());
    }
}

void Index::addDocument(uint64_t document_id, Index::DocumentInfo info, Index::IndexPerDocumentType&& document_index) {
    document_index_[document_id] = std::move(info);

    for (auto& [term, entry] : document_index) {
        entry.doc_id = document_id;
        inverted_index_[term].push_back(std::move(entry));
    }
}

void Index::enrichInvertedIndex(std::unordered_map<std::string, std::vector<Index::TermEntry>>&& new_data) {
    for (auto& [term, entries] : new_data) {
        inverted_index_[term].reserve(inverted_index_[term].size() + entries.size());
        std::move(entries.begin(), entries.end(), std::back_inserter(inverted_index_[term]));
    }
}

void Index::merge(Index&& other) {
    enrichInvertedIndex(std::move(other.inverted_index_));

    document_index_.insert(std::make_move_iterator(other.document_index_.begin()),
                           std::make_move_iterator(other.document_index_.end()));
}

std::vector<uint64_t> Index::query(const std::string& term) const {
    return queryInvertedIndex<uint64_t>(term, [](const TermEntry& entry) { return entry.doc_id; });
}

std::vector<std::string> Index::queryWithEnrichedResult(const std::string& term) const {
    return queryInvertedIndex<std::string>(term, [this](const TermEntry& entry) {
        return document_index_.at(entry.doc_id).header;
    });
}

std::string Index::getDocumentHeader(uint64_t document_id) const {
    return document_index_.at(document_id).header;
}

void Index::serialize(std::ostream& stream) const {
    stream << "Inverted index" << '\n';

    for (const auto& [term, entries] : inverted_index_) {
        stream << "Term: " << term << '\n';

        stream << "Entries: " << '\n';
        for (const auto& entry : entries) {
            stream << "doc_id=" << entry.doc_id << ", entries: ";

            for (uint64_t entry_in_doc : entry.entries) {
                stream << entry_in_doc << ", ";
            }

            stream << '\n';
        }
    }

    stream << "Document index" << '\n';

    for (const auto& [doc_id, doc_info] : document_index_) {
        stream << "doc_id=" << doc_id << ", doc_name=" << doc_info.header << '\n';
    }
}

void Index::serializeBinary(std::ostream& stream) const {
    writeAsBinary(stream, inverted_index_.size());

    for (const auto& [term, entries] : inverted_index_) {
        writeStringAsBinary(stream, term);

        writeAsBinary(stream, entries.size());

        for (const auto& entry : entries) {
            writeAsBinary(stream, entry.doc_id);
            writeAsBinary(stream, entry.entry_count);

            writeAsBinary(stream, entry.entries.size());
            for (uint64_t entry_in_doc : entry.entries) {
                writeAsBinary(stream, entry_in_doc);
            }
        }
    }

    writeAsBinary(stream, document_index_.size());

    for (const auto& [doc_id, doc_info] : document_index_) {
        writeAsBinary(stream, doc_id);

        writeStringAsBinary(stream, doc_info.header);
        writeStringAsBinary(stream, doc_info.link);
    }
}

void Index::deserializeBinary(std::istream &stream) {
    uint64_t inverted_index_size;
    readFromBinary(stream, inverted_index_size);

    for (uint64_t i = 0; i < inverted_index_size; ++i) {
        std::string term;
        readStringFromBinary(stream, term);

        uint64_t term_entries_size;
        readFromBinary(stream, term_entries_size);

        inverted_index_[term].reserve(term_entries_size);

        for (uint64_t entry = 0; entry < term_entries_size; ++entry) {
            TermEntry current;
            readFromBinary(stream, current.doc_id);
            readFromBinary(stream, current.entry_count);

            uint64_t entry_count_per_doc;
            readFromBinary(stream, entry_count_per_doc);

            current.entries.reserve(entry_count_per_doc);

            for (uint64_t entry_per_doc = 0; entry_per_doc < entry_count_per_doc; ++entry_per_doc) {
                uint64_t idx;
                readFromBinary(stream, idx);
                current.entries.push_back(idx);
            }

            inverted_index_[term].push_back(std::move(current));
        }
    }

    uint64_t document_index_size;
    readFromBinary(stream, document_index_size);
    for (int i = 0; i < document_index_size; ++i) {
        uint64_t doc_id;
        readFromBinary(stream, doc_id);

        std::ignore = document_index_[doc_id];
        readStringFromBinary(stream, document_index_[doc_id].header);
        readStringFromBinary(stream, document_index_[doc_id].link);
    }
}

