#pragma once
#ifndef	ASSETS_HPP
# define ASSETS_HPP

# include "main.hpp"

void    ft_toLower(std::string &str);
void	trim_space_newline(std::string &line);
int		is_empty(std::string &line);
void	fix_spaces_in_line(std::string &line);
void	print_list_content(std::list<std::string> list, std::string title);
void	print_list_content(std::list<int> list, std::string title);

#endif