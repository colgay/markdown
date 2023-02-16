#include <iostream>
#include <fstream>
#include <regex>
#include <string>
#include <sstream>
#include <stdexcept>
#include <filesystem>

#ifdef __linux__
constexpr auto FLAG = std::regex::ECMAScript | std::regex::multiline;
#elif defined(_WIN32)
constexpr auto FLAG = std::regex::ECMAScript;
#endif

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

void ParseFile(std::string source, std::string output)
{
    std::ifstream ifs(source);
    if (!ifs.is_open())
    {
        std::cout << "Error opening file '" << source << "'" << std::endl;
        return;
    }

    std::string content((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));

    std::regex dashes_r("^\\s*---\\s*$", FLAG);
    std::sregex_iterator it_end;

    auto it = std::sregex_iterator(content.begin(), content.end(), dashes_r);
    if (it == it_end)
    {
        std::cout << "[ERROR] dashes --- not found 1" << std::endl;
        return;
    }
    int64_t dashes_start = it->position() + it->length();

    it++;
    if (it == it_end)
    {
        std::cout << "[ERROR] dashes --- not found 2" << std::endl;
        return;
    }
    int64_t dashes_end = it->position();

    //std::cout << content.substr(dashes_start, dashes_end - dashes_start) << std::endl;

    std::string inside_str = content.substr(dashes_start, dashes_end - dashes_start);
    std::string new_str = "\n";

    std::smatch sm;
    std::regex title_r("^title\\s*:\\s*(.+)\\s*$", FLAG);
    if (!std::regex_search(inside_str, sm, title_r))
    {
        std::cout << "[ERROR] no title found inside dashes block" << std::endl;
        return;
    }
    new_str += string_format("Title: %s\n", sm[1].str().c_str());

    std::regex date_r("^date\\s*:\\s*(\\d{4}-\\d{2}-\\d{2})\\s+(\\d{2}:\\d{2}:\\d{2})\\s*$", std::regex::ECMAScript);
    if (!std::regex_search(inside_str, sm, date_r))
    {
        std::cout << "[ERROR] no date found inside dashes block" << std::endl;
        return;
    }
    new_str += string_format("Date: %sT%s+0800\n", sm[1].str().c_str(), sm[2].str().c_str());

    std::regex tags_r("^\\s*tags\\s*:\\s*$((?:\\s*^\\s*-\\s*.+\\s*$)+)", FLAG);
    if (std::regex_search(inside_str, sm, tags_r))
    {
        new_str += "Tags:";

        std::string tags_str = sm[1].str();
        std::regex tag_r("(?:^\\[.+\\]\\s*$|^\\s*-\\s*(.+)\\s*$)", FLAG);

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

    std::regex cates_r("^\\s*categories\\s*:\\s*$((?:\\s*^\\s*-\\s*.+\\s*$)+)", FLAG);
    if (std::regex_search(inside_str, sm, cates_r))
    {
        new_str += "Category:";

        std::string cate_str = sm[1].str();
        std::regex cate_r("(?:^\\[.+\\]\\s*$|^\\s*-\\s*(.+)\\s*$)", FLAG);

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

    std::ofstream ofile(output);
    ofile << content;
}

int main(int argc, char* argv[])
{
    if (argc < 3)
    {
        std::cout << "You must specify input and output" << std::endl;
        return EXIT_FAILURE;
    }

    std::string source;
    std::string dest;

    if (argc == 4)
    {
        std::string arg = argv[1];
        source = argv[2];
        dest = argv[3];

        if (arg == "-d")
        {
            std::filesystem::path src_path = source;
            if (!std::filesystem::exists(src_path))
            {
                std::cout << "Source dir not exists" << std::endl;
                return EXIT_FAILURE;
            }

            std::filesystem::path dest_path = dest;
            if (!std::filesystem::exists(dest_path))
                std::filesystem::create_directory(dest_path);

            for (const auto& entry : std::filesystem::directory_iterator(src_path))
            {
                ParseFile(entry.path().string(), dest_path.string() + "/" + entry.path().filename().string());
            }
        }
    }
    else
    {
        ParseFile(argv[1], argv[2]);
    }
}