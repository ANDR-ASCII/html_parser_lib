#pragma once

#include "headers.h"
#include "attribute.h"

namespace HtmlParser
{

class Tag
{
public:
	const std::string& name() const;
	const std::string& value() const;
	const std::deque<Attribute>& allAttributes() const;

	// returns value of attribute
	// empty string if specified attribute not found
	const std::string attribute(const std::string &nameAttribute) const;

	void setName(const std::string & setTagName);
	void setAttributes(const std::deque<Attribute> & setAttributes);
	void setAttribute(const std::string &attributeName, const std::string &attributeValue);
	void setValue(const std::string &setValueArg);

private:
	std::string m_tagName;
	std::string m_tagValue;
	std::deque<Attribute> m_attributes;
};

}