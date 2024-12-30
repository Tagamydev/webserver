#pragma once
#ifndef	ASSETS_HPP
# define ASSETS_HPP

# include "main.hpp"

void    ft_toLower(std::string &str);
void	trim_space_newline(std::string &line);
void	trim_semicolon(std::string &line);
void	trim_curly_brackets(std::string &line);

int		is_empty(std::string &line);
void	fix_spaces_in_line(std::string &line);
void	print_vector_content(std::vector<std::string> vector, std::string title);
void	print_list_content(std::list<std::string> list, std::string title);
void	print_list_content(std::list<int> list, std::string title);
void	print_map_content(std::map<int, std::string> map, std::string title);

std::vector<std::string>	split_to_vector(std::string line, char sep);
std::list<std::string>	split_to_list(std::string line, char sep);

#endif