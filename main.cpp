#include "headers.h"
#include "tag_parser_private.h"
#include "attribute.h"

int main()
{
	HtmlParser::TagParserPrivate privateParser;
	std::string aDoubleQuoted = "a href=\"www.cyberforum.ru\"        class    =   \"aStyle\"    ";
	std::string aSingleQuoted = "a href = ' www.cyberforum.ru ' class = 'aStyle'   ";
	std::string aUnquoted = "a href   =   www.cyberforum.ru class   =    aStyle  ";

	std::cout << privateParser.attributeOfTag(aUnquoted, "href") << std::endl << std::endl;

	try
	{
		std::cout << "Double quoted attributes parsing test: \n";

		std::deque<HtmlParser::Attribute> dDoubleQuoted = privateParser.readAllTagAttributes(aDoubleQuoted);

		for (HtmlParser::Attribute const& a : dDoubleQuoted)
		{
			std::cout << "attribute name: " << a.name() << std::endl;
			std::cout << "attribute value: " << a.value() << std::endl;
		}

		std::cout << "\nSingle quoted attributes parsing test: \n";

		std::deque<HtmlParser::Attribute> dSingleQuoted = privateParser.readAllTagAttributes(aSingleQuoted);

		for (HtmlParser::Attribute const& a : dSingleQuoted)
		{
			std::cout << "attribute name: " << a.name() << std::endl;
			std::cout << "attribute value: " << a.value() << std::endl;
		}

		std::cout << "\nUnquoted attributes parsing test: \n";

		std::deque<HtmlParser::Attribute> dUnquoted = privateParser.readAllTagAttributes(aUnquoted);

		for (HtmlParser::Attribute const& a : dUnquoted)
		{
			std::cout << "attribute name: " << a.name() << std::endl;
			std::cout << "attribute value: " << a.value() << std::endl;
		}
	}
	catch (const std::exception& ex)
	{
		std::cout << ex.what() << std::endl;
	}

	std::getchar();
	return 0;
}