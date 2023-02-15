#include <iostream>
#include <fstream>
#include <regex>
#include <string>
#include <sstream>
#include <stdexcept>

template<typename ... Args>
std::string string_format(const std::string& format, Args ... args)
{
    int size_s = std::snprintf(nullptr, 0, format.c_str(), args ...) + 1; // Extra space for '\0'
    if (size_s <= 0) { throw std::runtime_error("Error during formatting."); }
    auto size = static_cast<size_t>(size_s);
    std::unique_ptr<char[]> buf(new char[size]);
    std::snprintf(buf.get(), size, format.c_str(), args ...);
    return std::string(buf.get(), buf.get() + size - 1); // We don't want the '\0' inside
}

int main(int argc, char* argv[])
{
    if (argc < 3)
    {
        std::cout << "You must specify input and output" << std::endl;
        return EXIT_FAILURE;
    }

	std::string source = argv[1];
    std::string dest = argv[2];

    std::ifstream ifs(source);
    if (!ifs.is_open())
    {
        std::cout << "Error opening file '" << source << "'" << std::endl;
        return EXIT_FAILURE;
    }

    if (dest.empty())
    {
        std::cout << "You have to specify output name" << std::endl;
        return EXIT_FAILURE;
    }

    std::string content((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));

    std::regex dashes_r("^\\s*---\\s*$");
    std::sregex_iterator it_end;

    auto it = std::sregex_iterator(content.begin(), content.end(), dashes_r);
    if (it == it_end)
    {
        std::cout << "[ERROR] dashes --- not found 1" << std::endl;
        return EXIT_FAILURE;
    }
    int64_t dashes_start = it->position() + it->length();

    it++;
    if (it == it_end)
    {
        std::cout << "[ERROR] dashes --- not found 2" << std::endl;
        return EXIT_FAILURE;
    }
    int64_t dashes_end = it->position();

    //std::cout << content.substr(dashes_start, dashes_end - dashes_start) << std::endl;

    std::string inside_str = content.substr(dashes_start, dashes_end - dashes_start);
    std::string new_str = "\n";
    
    std::smatch sm;
    std::regex title_r("^title\\s*:\\s*(.+)\\s*$");
    if (!std::regex_search(inside_str, sm, title_r))
    {
        std::cout << "[ERROR] no title found inside dashes block" << std::endl;
        return EXIT_FAILURE;
    }
    new_str += string_format("Title: %s\n", sm[1].str().c_str());

    std::regex date_r("^date\\s*:\\s*(\\d{4}-\\d{2}-\\d{2})\\s+(\\d{2}:\\d{2}:\\d{2})\\s*$");
    if (!std::regex_search(inside_str, sm, date_r))
    {
        std::cout << "[ERROR] no date found inside dashes block" << std::endl;
        return EXIT_FAILURE;
    }
    new_str += string_format("Date: %sT%s+0800\n", sm[1].str().c_str(), sm[2].str().c_str());

    std::regex tags_r("^\\s*tags\\s*:\\s*$((?:\\s*^\\s*-\\s*.+\\s*$)+)");
    if (std::regex_search(inside_str, sm, tags_r))
    {
        new_str += "Tags:";

        std::string tags_str = sm[1].str();
        std::regex tag_r("(?:^\\[.+\\]\\s*$|^\\s*-\\s*(.+)\\s*$)");

        for (auto it = std::sregex_iterator(tags_str.begin(), tags_str.end(), tag_r); it != std::sregex_iterator(); ++it)
        {
            auto ts = (*it)[1].str();

            if (ts[0] == '[' && ts[ts.length() - 1] == ']')
            {
                ts = ts.substr(1, ts.length() - 2);
                std::stringstream ss(ts);
                std::string tag;
                while (getline(ss, tag, ','))
                {
                    new_str += " " + tag + ",";
                }
            }
            else
            {
                new_str += " " + ts + ",";
            }
        }

        if (new_str[new_str.length() - 1] == ',')
        {
            new_str.erase(new_str.length() - 1);
        }

        new_str += "\n";
    }

    std::regex cates_r("^\\s*categories\\s*:\\s*$((?:\\s*^\\s*-\\s*.+\\s*$)+)");
    if (std::regex_search(inside_str, sm, cates_r))
    {
        new_str += "Category:";

        std::string cate_str = sm[1].str();
        std::regex cate_r("(?:^\\[.+\\]\\s*$|^\\s*-\\s*(.+)\\s*$)");

        for (auto it = std::sregex_iterator(cate_str.begin(), cate_str.end(), cate_r); it != std::sregex_iterator(); ++it)
        {
            auto cs = (*it)[1].str();

            if (cs[0] == '[' && cs[cs.length() - 1] == ']')
            {
                cs = cs.substr(1, cs.length() - 2);
                std::stringstream ss(cs);
                std::string cate;
                while (getline(ss, cate, ','))
                {
                    new_str += " " + cate + ",";
                }
            }
            else
            {
                new_str += " " + cs + ",";
            }
        }

        if (new_str[new_str.length() - 1] == ',')
        {
            new_str.erase(new_str.length() - 1);
        }

        new_str += "\n";
    }

    content.erase(dashes_start, dashes_end - dashes_start);
    content.insert(dashes_start, new_str);

    std::ofstream ofile(dest);
    ofile << content;
}