#include "search.h"
#include <string>
#include <cmath>

bool SearchEngine::CmpInsensitively::operator()(std::string_view lhv, std::string_view rhv) const {
    size_t n = std::min(lhv.size(), rhv.size());
    for (size_t i = 0; i < n; ++i) {
        if (lhv[i] >= 'A' && lhv[i] <= 'Z') {

            if (rhv[i] >= 'A' && rhv[i] <= 'Z') {
                if (lhv[i] != rhv[i]) {
                    return lhv[i] < rhv[i];
                }
                continue;
            }

            if (lhv[i] + 'a' - 'A' != rhv[i]) {
                return lhv[i] + 'a' - 'A' < rhv[i];
            }
            continue;
        }

        if (rhv[i] >= 'a' && rhv[i] <= 'z') {
            if (lhv[i] != rhv[i]) {
                return lhv[i] < rhv[i];
            }
            continue;
        }

        if (lhv[i] != rhv[i] + 'a' - 'A') {
            return lhv[i] < rhv[i] + 'a' - 'A';
        }
    }
    return lhv.size() < rhv.size();
}

void SearchEngine::FillRows(std::string_view text) {

    auto left = text.begin();

    for (auto right = text.begin(); right != text.end(); ++right) {

        if (*right == '\n') {
            if (right - left > 1) {
                rows_.emplace_back(left, right);
            }
            left = right + 1;
        }
    }

    if (text.end() - left > 0) {
        rows_.emplace_back(left, text.end());
    }
}

void SearchEngine::FillMapOfWords(size_t i, InsensitiveMap& query_word_to_quantity, size_t& total_words) {
    size_t left = 0;

    for (size_t right = 0; right < rows_[i].size(); ++right) {
        if (!std::isalpha(rows_[i][right])) {
            if (right - left > 0) {
                query_word_to_quantity[rows_[i].substr(left, right - left)] += 1;
                ++total_words;
            }
            left = right + 1;
        }
    }

    if (left < rows_[i].size()) {
        query_word_to_quantity[rows_[i].substr(left, rows_[i].size() - left)] += 1;
        ++total_words;
    }
}

void SearchEngine::FillRowIdToRowInfoMap() {

    for (size_t i = 0; i < rows_.size(); ++i) {

        size_t total_words_in_row = 0;
        InsensitiveMap row_info;

        FillMapOfWords(i, row_info, total_words_in_row);

        row_id_to_row_info_[i] = std::make_pair(row_info, total_words_in_row);
    }
}

void SearchEngine::BuildIndex(std::string_view text) {
    FillRows(text);
    FillRowIdToRowInfoMap();
}

size_t SearchEngine::CountWordQuantityInDocumentCollection(std::string_view word) const {

    size_t count = 0;

    for (auto& [id, word_to_quantity__and__total] : row_id_to_row_info_) {

        if (word_to_quantity__and__total.first.contains(word)) {
            ++count;
        }
    }
    return count;
}

void SearchEngine::FillQueryWordsOccurredInDocToQuantityAmongDocCollection(
    std::string_view query, InsensitiveMap& occurred_query_word_to_quantity) const {

    size_t left = 0;
    size_t right = 0;

    for (; right < query.size(); ++right) {
        if (!std::isalpha(query[right])) {
            if (right - left > 0) {

                std::string_view query_word = query.substr(left, right - left);

                if (!occurred_query_word_to_quantity.contains(query_word)) {

                    size_t quantity = CountWordQuantityInDocumentCollection(query_word);

                    if (quantity > 0) {
                        occurred_query_word_to_quantity[query_word] = quantity;
                    }
                }
            }
            left = right + 1;
        }
    }

    if (left < query.size()) {
        std::string_view query_word = query.substr(left, right - left);

        if (!occurred_query_word_to_quantity.contains(query_word)) {

            size_t quantity = CountWordQuantityInDocumentCollection(query_word);

            if (quantity > 0) {
                occurred_query_word_to_quantity[query_word] = quantity;
            }
        }
    }
}

bool CompareDouble(const std::pair<double, size_t>& lhv, const std::pair<double, size_t>& rhv) {
    if (std::fabs(rhv.first - lhv.first) >= std::numeric_limits<double>::epsilon()) {
        return lhv.first >= rhv.first;
    }
    return lhv.second < rhv.second;
}

double TfIdf(size_t quantity_in_row, size_t total_in_row, size_t quantity_in_document_collection,
             size_t total_documents) {
    return (static_cast<double>(quantity_in_row) / static_cast<double>(total_in_row)) *
           (log(static_cast<double>(total_documents) / static_cast<double>(quantity_in_document_collection)));
}

void SearchEngine::FillTopRelevant(std::vector<std::pair<double, size_t>>& top_relevant,
                                   const InsensitiveMap& occurred_query_word_to_quantity) const {

    for (auto& [id, word_to_quantity__and__total] : row_id_to_row_info_) {
        double relevance = 0.0;

        for (auto& [word, quantity] : occurred_query_word_to_quantity) {
            if (word_to_quantity__and__total.first.contains(word)) {

                if (quantity != rows_.size()) {
                    relevance += TfIdf(word_to_quantity__and__total.first.at(word), word_to_quantity__and__total.second,
                                       quantity, rows_.size());
                }
            }
        }

        if (std::fabs(relevance) >= std::numeric_limits<double>::epsilon()) {
            top_relevant.emplace_back(relevance, id);
        }
    }

    std::stable_sort(top_relevant.begin(), top_relevant.end(), CompareDouble);
}

std::vector<std::string_view> SearchEngine::Search(std::string_view query, size_t results_count) const {

    InsensitiveMap occurred_query_word_to_quantity;  // for IDF also
    FillQueryWordsOccurredInDocToQuantityAmongDocCollection(query, occurred_query_word_to_quantity);

    std::vector<std::pair<double, size_t>> top_relevant;
    FillTopRelevant(top_relevant, occurred_query_word_to_quantity);

    std::vector<std::string_view> result;
    size_t result_size = std::min(top_relevant.size(), results_count);
    result.reserve(result_size);

    for (size_t i = 0; i < result_size; ++i) {
        result.emplace_back(rows_[top_relevant[i].second]);
    }

    return result;
}
