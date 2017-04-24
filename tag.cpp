#include "headers.h"
#include "tag.h"
#include "common.h"

namespace HtmlParser
{

const std::string& Tag::name() const
{
	return m_tagName;
}

const std::string& Tag::value() const
{
	return m_tagValue;
}

const std::deque<HtmlParser::Attribute>& Tag::allAttributes() const
{
	return m_attributes;
}

// returns value of attribute
// empty string if occurs error
const std::string Tag::attribute(const std::string &nameAttribute) const
{
	if (nameAttribute.empty())
	{
		return "";
	}

	int countAttrs = m_attributes.size();
	for (int i = 0; i < countAttrs; i++)
	{
		if (Common::strToLower(m_attributes[i].name()) == Common::strToLower(nameAttribute))
		{
			return m_attributes[i].value();
		}
	}

	return "";
}

void Tag::setName(const std::string& setTagName)
{
	m_tagName = setTagName;
}

void Tag::setAttributes(const std::deque<Attribute>& setAttributes)
{
	m_attributes = setAttributes;
}

void Tag::setAttribute(const std::string& attributeName, const std::string& attributeValue)
{
	m_attributes.push_back(Attribute(attributeName, attributeValue));
}

void HtmlParser::Tag::setValue(const std::string& setValueArg)
{
	m_tagValue = setValueArg;
}

}
