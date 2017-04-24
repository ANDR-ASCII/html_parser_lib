#include "headers.h"
#include "attribute.h"
#include "tag_parser_private.h"
#include "common.h"

namespace HtmlParser
{

const std::string TagParserPrivate::s_singleTags[] =
{
	"area",
	"base",
	"basefont",
	"bgsound",
	"br",
	"col",
	"command",
	"embed",
	"frame",
	"hr",
	"img",
	"input",
	"isindex",
	"keygen",
	"source",
	"track",
	"wbr",
	"link",
	"meta",
	"param",
	"!doctype"
};

const std::size_t TagParserPrivate::s_singleTagsNumber = sizeof(s_singleTags) / sizeof(s_singleTags[0]);

/*********************************************************************************************************/

void TagParserPrivate::skipWhitespaces(const std::string& str, std::size_t& position) const
{
	const std::size_t stringLength = str.size();
	std::size_t idx = position;

	for (; idx < stringLength; ++idx)
	{
		if (!isSpace(str[idx]))
		{
			break;
		}
	}
	position = idx;
}

bool TagParserPrivate::isDoubleQuotes(char ch) const
{
	if (ch == static_cast<char>(Symbols::DoubleQuotes))
	{
		return true;
	}

	return false;
}

bool TagParserPrivate::isSingleQuote(char ch) const
{
	if (ch == static_cast<char>(Symbols::SingleQuote))
	{
		return true;
	}

	return false;
}

bool TagParserPrivate::isEqualCharacter(char ch) const
{
	if (ch == static_cast<char>(Symbols::Equally))
	{
		return true;
	}

	return false;
}

bool TagParserPrivate::isOpenTag(char ch) const
{
	if (ch == static_cast<char>(Symbols::OpenTag))
	{
		return true;
	}

	return false;
}

bool TagParserPrivate::isCloseTag(char ch) const
{
	if (ch == static_cast<char>(Symbols::CloseTag))
	{
		return true;
	}

	return false;
}

bool TagParserPrivate::isNewLine(char ch) const
{
	if (ch == static_cast<char>(Symbols::NewLine))
	{
		return true;
	}

	return false;
}

bool TagParserPrivate::isSpace(char ch) const
{
	return std::isspace(static_cast<unsigned char>(ch));
}

bool TagParserPrivate::isAlpha(char ch) const
{
	return std::isalpha(static_cast<unsigned char>(ch));
}

bool TagParserPrivate::isAlnum(char ch) const
{
	return std::isalnum(static_cast<unsigned char>(ch));
}

bool TagParserPrivate::skippedCommentaryTag(const std::string& htmlPage, std::size_t& position) const
{
	const auto size = htmlPage.size();
	std::string isEndCommentTag;

	if (isCommentaryTag(htmlPage, position))
	{
		// shifts pointer to the open tag character of commentary tag
		position += 3;

		while (position < size)
		{
			// if find character '-', write it and two next characters
			if (htmlPage[position] == '-')
			{
				isEndCommentTag  = htmlPage[position];
				isEndCommentTag += htmlPage[position + 1];
				isEndCommentTag += htmlPage[position + 2];

				// shifts ptr of the character close tag and return result
				if (isEndCommentTag == "-->")
				{
					position += 3;
					return true;
				}
				else
				{
					isEndCommentTag.clear();
				}
			}

			++position;
		}
	}

	return false;
}

bool TagParserPrivate::isCommentaryTag(const std::string& htmlPage, std::size_t& position) const
{
	if (position > htmlPage.size() || htmlPage.empty())
	{
		return false;
	}

	if (htmlPage[position] == '!' && htmlPage[position + 1] == '-' && htmlPage[position + 2] == '-')
	{
		return true;
	}

	return false;
}

std::string TagParserPrivate::readTag(const std::string& htmlPage, std::size_t& position) const
{
	const auto size = htmlPage.size();
	std::string tag;

	while (position < size)
	{
		// if found character of open tag
		if (isOpenTag(htmlPage[position++]))
		{
			if (skippedCommentaryTag(htmlPage, position))
			{
				//
				// skip the iteration of cycle after ignore commentary tag
				// otherwise: <!-- some text --><sometag> - sometag cannot be read
				// because the next iteration of cycle attempts to write the name of tag and the result "<nameTag"
				// and after that function tagName returns empty string.
				//
				continue;
			}

			// write all characters up to the first character of close tag.
			while (position < size && !isCloseTag(htmlPage[position]))
			{
				tag += htmlPage[position++];
			}

			// skip character ">"
			++position;
			break;
		}
	}

	return tag;
}

std::string TagParserPrivate::readValue(const std::string& htmlPage, std::size_t& position) const
{
	std::string value;
	const auto size = htmlPage.size();

	while (position < size)
	{
		if (!isOpenTag(htmlPage[position]))
		{
			value += htmlPage[position++];
		}
		else
		{
			break;
		}
	}

	return value;
}

std::string TagParserPrivate::readAllUpToTag(const std::string& htmlPage, const std::string& tag, std::size_t& position) const
{
	std::string value = readValue(htmlPage, position);
	std::string currentTagName = Common::strToLower(readTag(htmlPage, position));

	const std::size_t size = htmlPage.size();

	while (position < size && tagName(currentTagName) != Common::strToLower(tag))
	{
		currentTagName = static_cast<char>(Symbols::OpenTag) + currentTagName + static_cast<char>(Symbols::CloseTag);
		value += currentTagName;

		value += readValue(htmlPage, position);
		currentTagName = Common::strToLower(readTag(htmlPage, position));
	}

	return value;
}

//
// TODO: refactor
//
std::string TagParserPrivate::attributeOfTag(const std::string& tagString, const std::string& attribute) const
{
	const std::string::size_type idx = isSetTagAttribute(tagString, attribute);

	const bool isArgumentInvalid = tagString.empty() || attribute.empty();
	const bool isAttributeFound = idx != std::string::npos;

	std::string attributeValue;

	if (isArgumentInvalid)
	{
		throw std::invalid_argument("Empty argument");
	}

	if (isAttributeFound)
	{
		const int attributeLength = attribute.size();
		const int lastCharacterAttributeNamePosition = idx + attributeLength;
		const int tagStringLength = tagString.size();

		int equalCharacterPosition = 0;

		// skip all the characters up to the first whitespace
		for (int i = lastCharacterAttributeNamePosition; i < tagStringLength; ++i)
		{
			if (isSpace(tagString[i]))
			{
				continue;
			}
			else if(!isEqualCharacter(tagString[i]))
			{
				throw std::logic_error("Before character '=' appeared character which is not a whitespace");
			}
			else
			{
				equalCharacterPosition = i;
				break;
			}
		}

		int quotesCharacterPosition = 0;
		bool attributeValueQuoted = false;

		// skip all characters up to the first quotes
		for (int i = equalCharacterPosition + 1; i < tagStringLength; ++i)
		{
			if (isSpace(tagString[i]))
			{
				continue;
			}
			else
			{
				if (isDoubleQuotes(tagString[i]) || isSingleQuote(tagString[i]))
				{
					quotesCharacterPosition = i;
					attributeValueQuoted = true;
				}

				break;
			}
		}

		//**
		//***********************************************************************************************************
		//**

		// I seem this is a bad style programming
		// but I suppose this code will see only am I

		using checkCharacterFunctionTypePointer = bool(TagParserPrivate::*)(char) const;

		checkCharacterFunctionTypePointer checkQuotesFunction = isDoubleQuotes(tagString[quotesCharacterPosition]) ?
			&TagParserPrivate::isDoubleQuotes : &TagParserPrivate::isSingleQuote;

		//**
		//***********************************************************************************************************
		//**

		// if found quotes then write all after it up to the first quotes character

		bool attributeValueClosedByQuotes = false;

		for (int i = quotesCharacterPosition + 1; i < tagStringLength; ++i)
		{
			if (attributeValueQuoted && !(this->*checkQuotesFunction)(tagString[i]) ||
				!attributeValueQuoted && !isSpace(tagString[i]))
			{
				attributeValue += tagString[i];
			}
			else
			{
				attributeValueClosedByQuotes = attributeValueQuoted;
				break;
			}
		}

		return !attributeValueClosedByQuotes && attributeValueQuoted ?
			throw std::logic_error("Attribute value must be closed by quotes") : attributeValue;
	}

	// attribute not found
	return std::string();
}

//
// TODO: check validity
//
std::string TagParserPrivate::tagName(const std::string& tagString) const
{
	const std::size_t size = tagString.size();

	if (tagString.empty())
	{
		throw std::invalid_argument("Empty tag string");
	}

	std::size_t position = 0;

	return readUntil(tagString, position, [this](char ch) { return !isSpace(ch); });
}

//
// TODO: refactor
//
std::string::size_type TagParserPrivate::isSetTagAttribute(const std::string& tagString, const std::string& attribute) const
{
	const auto idx = tagString.find(attribute);

	return idx != std::string::npos && idx >= 2 && isSpace(tagString[idx - 1]) ? idx : std::string::npos;
}

std::deque<Attribute> TagParserPrivate::readAllTagAttributes(const std::string& tagString) const
{
	const std::size_t tagStringLength = tagString.size();
	const std::size_t tagNameLength = tagName(tagString).size();

	std::deque<Attribute> attributes;

	// if exists attributes, read it
	if (isSetTagAttributes(tagString))
	{
		Attribute attribute;

		std::size_t idx = tagNameLength;

		skipWhitespaces(tagString, idx);

		while(idx < tagStringLength)
		{
			// write attribute name
			attribute.setName(readUntil(tagString, idx, [this](char ch) { return !isSpace(ch) && !isEqualCharacter(ch); }));

			skipWhitespaces(tagString, idx);

			//
			// check validity of HTML attribute syntax
			// and increment position to the next character
			// which is should be alphabetical character or 
			// single/double quotes character
			//
			if (!isEqualCharacter(tagString[idx++]))
			{
				throw std::logic_error("Tag string do not contain '=' character after attribute name");
			}

			skipWhitespaces(tagString, idx);

			if (!(isDoubleQuotes(tagString[idx]) || isSingleQuote(tagString[idx])))
			{
				attribute.setValue(readUntil(tagString, idx, [this](char ch) { return !isSpace(ch); }));
			}
			else
			{
				//**
				//***********************************************************************************************************
				//**

				// I seem this is a bad style programming
				// but I suppose this code will see only am I

				using checkCharacterFunctionTypePointer = bool(TagParserPrivate::*)(char) const;

				checkCharacterFunctionTypePointer checkQuotesFunction = isDoubleQuotes(tagString[idx]) ?
					&TagParserPrivate::isDoubleQuotes : &TagParserPrivate::isSingleQuote;

				//**
				//***********************************************************************************************************
				//**

				attribute.setValue(readUntil(tagString, ++idx,
					[this, checkQuotesFunction](char ch) { return !(this->*checkQuotesFunction)(ch); }));

				++idx;
			}

			skipWhitespaces(tagString, idx);

			attributes.push_back(attribute);
		}
	}

	return attributes;
}

//
// TODO: refactor
//
bool TagParserPrivate::isSetTagAttributes(const std::string& tagString) const
{
	const auto idx = tagString.find(static_cast<char>(Symbols::Equally));
	auto i = 0ul;

	if (idx != std::string::npos)
	{
		i = idx - 1;

		if (idx <= 2)
		{
			return false;
		}

		// skip all white-spaces between character = and last letter of name attribute
		do
		{
			--i;

		} while (isSpace(tagString[i]) && i > 0);

		// skip name of attribute until meet white-space characters
		do
		{
			--i;

		} while (!isSpace(tagString[i]) && i > 0);

		// test second part after character =
		if (isSpace(tagString[i]))
		{
			i = idx + 1;

			// if after = meet " or ' or letter/digit, return true
			if (isDoubleQuotes(tagString[i]) || isSingleQuote(tagString[i]) || isAlnum(tagString[i]))
			{
				return true;
			}

			// skip white-spaces
			do
			{
				++i;
			} while (isSpace(tagString[i]) && i > 0);

			// if find double or single quotes or letter/digit, return true
			if (isDoubleQuotes(tagString[i]) || isSingleQuote(tagString[i]) || isAlnum(tagString[i]))
			{
				return true;
			}
			// otherwise tag invalid
			else
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}

	return false;
}

bool TagParserPrivate::isSingleTag(const std::string& tagName) const
{
	for (std::size_t i = 0; i < s_singleTagsNumber; i++)
	{
		if (Common::strToLower(tagName) == s_singleTags[i])
		{
			return true;
		}
	}

	return false;
}

}