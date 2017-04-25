#include "headers.h"
#include "tag_parser.h"
#include "common.h"
#include "tag_parser_private.h"

namespace HtmlParser
{

D_FUNCTION_IMPL(TagParser)


TagParser::TagParser()
	: m_inviteMode(false)
{
}

TagParser::TagParser(const TagParser& parser)
{
	m_inviteMode = parser.m_inviteMode;
	clear();

	const auto tagsNumber = parser.count();

	for (auto i = 0ul; i < tagsNumber; ++i)
	{
		m_tags.push_back(parser[i]);
	}
}

TagParser::iterator TagParser::begin()
{
	return m_tags.begin();
}

TagParser::iterator TagParser::end()
{
	return m_tags.end();
}

TagParser::const_iterator TagParser::begin() const
{
	return m_tags.begin();
}

TagParser::const_iterator TagParser::end() const
{
	return m_tags.end();
}

TagParser::const_iterator TagParser::cbegin() const
{
	return m_tags.cbegin();
}

TagParser::const_iterator TagParser::cend() const
{
	return m_tags.cend();
}

TagParser& TagParser::operator=(const TagParser& parser)
{
	if (this == &parser)
	{
		return *this;
	}

	m_inviteMode = parser.m_inviteMode;
	clear();

	const auto tagsNumber = parser.count();

	for (auto i = 0ul; i < tagsNumber; ++i)
	{
		m_tags.push_back(parser[i]);
	}

	return *this;
}

void TagParser::parsePageByTags(const std::string& htmlPage)
{
	D_FUNCTION(TagParser);

	testInviteMode();

	if (htmlPage.empty())
	{
		throw std::invalid_argument("Empty argument");
	}

	std::string tag;
	std::string tagValue;

	const std::size_t lengthOfPage = htmlPage.size();

	std::size_t pointer = 0u;
	std::size_t mem = 0u;

	while (pointer < lengthOfPage)
	{
		Tag tmpTag;
		Attribute tmpAttr;

		tag = m_d->readTag(htmlPage, pointer);

		tmpTag.setName(Common::strToLower(m_d->tagName(tag)));
		tmpTag.setAttributes(m_d->readAllTagAttributes(tag));

		if (tag[0] != '/')
		{
			if (!m_d->isSingleTag(m_d->tagName(tag)))
			{
				mem = pointer;
				tagValue = m_d->readAllUpToTag(htmlPage, "/" + m_d->tagName(tag), pointer);
				tmpTag.setValue(tagValue);
				pointer = mem;
			}

			m_tags.push_back(tmpTag);
		}
	}
}

// parse specified tags from first argument
void TagParser::parseTags(const std::string& htmlPage, const std::string& tagName, bool parseAttributes)
{
	D_FUNCTION(TagParser);

	testInviteMode();

	if (tagName.empty() || htmlPage.empty() || tagName == "!--")
	{
		return;
	}

	decltype(htmlPage.size()) position = 0, size = htmlPage.size();

	// text of read the tag like this - tag [attribute1="value1" attribute2="value2"]
	std::string tagText;
	std::deque<Attribute> attributesOfNameTag;

	// name of current read tag
	std::string nameOfReadedTag, nameTagCmp = Common::strToLower(tagName);


	while (position < size)
	{
		// initializing object of Tag
		Tag tmpTag;
		tagText = m_d->readTag(htmlPage, position);

		// if find the specified tag, write it
		if (Common::strToLower(m_d->tagName(tagText)) == nameTagCmp)
		{
			tmpTag.setName(nameTagCmp);

			// if need parse attributes
			if (parseAttributes)
			{
				tmpTag.setAttributes(m_d->readAllTagAttributes(tagText));
			}

			m_tags.push_back(tmpTag);
		}
	}
}

// parse specified tags with value from first argument
void TagParser::parseTagsWithValue(const std::string& htmlPage, const std::string& tagName, bool parseAttributes)
{
	D_FUNCTION(TagParser);

	testInviteMode();

	if (tagName.empty() || htmlPage.empty() || tagName == "!--")
	{
		return;
	}

	std::size_t position = 0, size = htmlPage.size();

	std::string tagText, nameTagCmp = Common::strToLower(tagName);
	std::deque<Attribute> attributesOfNameTag;

	while (position < size)
	{
		Tag tmpTag;
		tagText = m_d->readTag(htmlPage, position);

		if (Common::strToLower(m_d->tagName(tagText)) == nameTagCmp)
		{
			tmpTag.setName(nameTagCmp);

			// if need parse attributes of tag
			if (parseAttributes)
			{
				tmpTag.setAttributes(m_d->readAllTagAttributes(tagText));
			}

			tmpTag.setValue(m_d->readAllUpToTag(htmlPage, "/" + m_d->tagName(tagText), position));

			m_tags.push_back(tmpTag);
		}
	}
}

Tag const& TagParser::operator[](std::size_t i) const
{
	std::size_t countTags = m_tags.size();

	if (i > countTags || !countTags)
	{
		throw std::out_of_range("Error indexing array");
	}

	return m_tags[i];
}

std::size_t TagParser::size() const
{
	return m_tags.size();
}

std::size_t TagParser::countTag(const std::string& tag) const
{
	std::size_t number = size(), numberOfTagsFound = 0;

	if (number > 0)
	{
		for (std::size_t i = 0; i < number; ++i)
		{
			if (m_tags[i].name() == tag)
			{
				numberOfTagsFound++;
			}
		}

		return numberOfTagsFound;
	}
	else
	{
		return 0;
	}
}

// clear queue of tags if turned off inviteMode
void TagParser::testInviteMode()
{
	if (!m_inviteMode)
	{
		clear();
	}
}

void TagParser::clear()
{
	m_tags.clear();
}

void TagParser::setInviteMode(bool inviteMode)
{
	m_inviteMode = inviteMode;
}

}