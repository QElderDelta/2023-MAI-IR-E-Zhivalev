#pragma once

#include <functional>
#include <vector>
#include <unordered_map>

class Index {
public:
    struct TermEntry {
        uint64_t doc_id{0};
        uint64_t entry_count{0};
        std::vector<uint64_t> entries;
    };

    struct DocumentInfo {
        std::string header;
        std::string link;
    };

    using IndexPerDocumentType = std::unordered_map<std::string, TermEntry>;

    // Precondition: document_id must be greater than all of document ids in the index
    void addDocument(uint64_t document_id, DocumentInfo info, IndexPerDocumentType&& document_index);

    // Precondition: for each term max(doc_id) from current index must be less than min(doc_id) from other index
    void merge(Index&& other);

    std::vector<uint64_t> query(const std::string& term) const;

    std::vector<std::string> queryWithEnrichedResult(const std::string& term) const;

    std::string getDocumentHeader(uint64_t document_id) const;

    void serialize(std::ostream& stream) const;

    void serializeBinary(std::ostream& stream) const;
    void deserializeBinary(std::istream& stream);
private:
    template <typename ResultType, typename EntryProcessor>
    std::vector<ResultType> queryInvertedIndex(const std::string& term, const EntryProcessor& processor) const {
        auto it = inverted_index_.find(term);

        if (it == inverted_index_.end()) {
            return {};
        }

        std::vector<ResultType> result;
        result.reserve(it->second.size());

        for (const auto& doc : it->second) {
            result.push_back(processor(doc));
        }

        return result;
    }

    using InvertedIndexType = std::unordered_map<std::string, std::vector<Index::TermEntry>>;

    void enrichInvertedIndex(std::unordered_map<std::string, std::vector<Index::TermEntry>>&& new_data);

    InvertedIndexType inverted_index_;
    std::unordered_map<uint64_t, DocumentInfo> document_index_;
};
