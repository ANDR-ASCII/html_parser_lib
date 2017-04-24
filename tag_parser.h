#pragma once

#include "headers.h"
#include "tag.h"

namespace HtmlParser
{

class TagParserPrivate;

class TagParser
{
	std::deque<Tag> m_tags;
	bool m_inviteMode;

	// clear queue of tags if turned off inviteMode
	void testInviteMode();

	D_FUNCTION_DECL(TagParser);

public:
	using size_type = std::deque<std::string>::size_type;

	TagParser();
	TagParser(const TagParser &parser);

	TagParser &operator=(const TagParser &parser);

	// in order to support call range-for operator
	auto begin() const -> decltype(m_tags.begin()) { return m_tags.begin(); }
	auto end() const -> decltype(m_tags.end()) { return m_tags.end(); }

	// parse XML page by tags
	void parseByTags(const std::string &filename);

	// parse specified tags from first argument
	void parseTags(const std::string &xmlText, const std::string &nameTag, bool parseAttributes = true);

	// parse specified tags with value from first argument
	void parseTagsWithValue(const std::string &xmlText, const std::string &nameTag, bool parseAttributes = true);

	const Tag &operator[](size_type i) const;

	size_type count() const { return m_tags.size(); }
	size_type size() const { return m_tags.size(); }

	size_type countTag(const std::string &tag) const;
	void clear();
	void setInviteMode(bool setInvMode);
};
	
}