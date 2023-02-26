#pragma once

#include <string_view>
#include <vector>
#include <map>
#include <set>

class SearchEngine {
public:
    struct CmpInsensitively {
        bool operator()(std::string_view lhv, std::string_view rhv) const;
    };
    using InsensitiveMap = std::map<std::string_view, size_t, CmpInsensitively>;
    using RowIdToRowInfoMap = std::map<size_t, std::pair<InsensitiveMap, size_t>>;

    void BuildIndex(std::string_view text);
    std::vector<std::string_view> Search(std::string_view query, size_t results_count) const;

private:
    void FillRows(std::string_view text);
    void FillRowIdToRowInfoMap();
    void FillMapOfWords(size_t i, InsensitiveMap& query_word_to_quantity, size_t& total_words);
    size_t CountWordQuantityInDocumentCollection(std::string_view word) const;
    void FillQueryWordsOccurredInDocToQuantityAmongDocCollection(std::string_view query,
                                                                 InsensitiveMap& occurred_query_word_to_quantity) const;
    void FillTopRelevant(std::vector<std::pair<double, size_t>>& top_relevant,
                         const InsensitiveMap& occurred_query_word_to_quantity) const;

    std::vector<std::string_view> rows_;
    RowIdToRowInfoMap row_id_to_row_info_;
};
