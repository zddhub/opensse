#ifndef SEARCHENGINE_H
#define SEARCHENGINE_H

#include <string>
#include <vector>

typedef unsigned int uint;

struct QueryResult
{
        std::string imageName;
        uint imageIndex;
        float ratio;

        bool operator< (const QueryResult& r) const
        {
                return ratio > r.ratio;
        }
};

typedef std::vector<QueryResult> QueryResults;

/**
 * @brief The SearchEngine class
 * Search engine interface
 */
class SearchEngine
{
public:
    virtual void query(const std::string &fileName, QueryResults& results) = 0;
};

#endif // SEARCHENGINE_H
