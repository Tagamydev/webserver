/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   assets.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: samusanc <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/11 08:36:52 by samusanc          #+#    #+#             */
/*   Updated: 2024/12/14 12:36:46 by daviles-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.hpp"


/// @brief Split line by sep, return in a string vector.
/// @param line content to split
/// @param sep separator to split by
/// @return std::vector<std::string>
std::vector<std::string>	split_to_vector(std::string line, char sep)
{
	std::vector<std::string>	vector;

	while (!line.empty())
	{
		if (line.find(sep) != std::string::npos)
		{
			vector.push_back(line.substr(0, line.find(sep)));
			line.erase(0, line.find(sep) + 1);
		}
		else
		{
			vector.push_back(line.substr(0, line.length()));
			line.erase(0, line.length());
		}
	}
	return (vector);
}


/// @brief Split line by sep, return in a string list.
/// @param line content to split
/// @param sep separator to split by
/// @return std::list<std::string>
std::list<std::string>	split_to_list(std::string line, char sep)
{
	std::list<std::string>	list;

	while (!line.empty())
	{
		if (line.find(sep) != std::string::npos)
		{
			list.push_back(line.substr(0, line.find(sep)));
			line.erase(0, line.find(sep) + 1);
		}
		else
		{
			list.push_back(line.substr(0, line.length()));
			line.erase(0, line.length());
		}
	}
	return (list);
}

/// @brief transform a string in lower case
void    ft_toLower(std::string &str)
{
    for (size_t i = 0; i < str.length(); ++i)
        str[i] = std::tolower(str[i]);
}

void	trim_curly_brackets(std::string &line)
{
	int i = 0;
	int j = line.length() - 1;

	while (line[i] != '\0' && line[i] == '{')
		i++;
	while (line[j] != '\0' && line[j] == '}')
		j--;
	line = line.substr(i,j - i +1);
}

/// @brief Trim semicolon from the end on a string. Used to clean values on parameters.
/// @param line - Takes the pointer
void trim_semicolon(std::string &line)
{
	int i = 0;
	int j = line.length() - 1;

	while (line[i] != '\0' && line[i] == ';')
		i++;
	while (line[j] != '\0' && line[j] == ';')
		j--;
	line = line.substr(i,j - i +1);
}

/// @brief Trim spaces and new lines from the start and from the end on a string.
/// @param line - Takes the pointer
void trim_space_newline(std::string &line)
{
	int i = 0;
	int j = line.length() - 1;
	int	result;

	while (line[i] != '\0' && (line[i] == ' ' || line[i] == '\t' || line[i] == '\r' || line[i] == '\f' || line[i] == '\n'))
		i++;
	// this line needs to be added, j = -1 in one case
	if (j < 0)
		j = 0;
	while (line[j] != '\0' && (line[j] == ' ' || line[j] == '\t' || line[j] == '\r' || line[j] == '\f' || line[j] == '\n'))
	{
		j--;
	}
	line = line.substr(i,j - i +1);
}

/// @brief Check if content is empty, or only have spaces, tabs or \\n
/// @param line 
/// @return true if it is empty
int is_empty(std::string &line)
{
	int	i = 0;

	if (line.empty())
		return (1);
	while (line[i] != '\0' && line[i] == ' ' || line[i] == '\n' || line[i] == '\t')
		i++;
	if (line.length() == i)
		return (1);
	return (0);
}

/// @brief Skip is_space at begining. Replace many spaces or tabs to 1 space.
/// @param line takes the pointer
void fix_spaces_in_line(std::string &line)
{
	std::string parsedLine;
	int i = 0;

	if (line[0] == ' ' || line[0] == '\t' || line[0] == '\r' || line[0] == '\f' || line[0] == '\n')
		while (line[i] == ' ' || line[i] == '\t' || line[i] == '\r' || line[i] == '\f' || line[i] == '\n')
			i++;
	while (i != line.length() || line[i] != '\0')
	{
		if (line[i] == ' ' || line[i] == '\t' || line[i] == '\r' || line[i] == '\f')
		{
			while (line[i] == ' ' || line[i] == '\t' || line[i] == '\r' || line[i] == '\f')
			   i++;
			if (line[i] == '\0')
				break;
			parsedLine += ' ';
		}
		parsedLine += line[i];
		i++;
	}
	line = parsedLine;
}

void	print_map_content(std::map<int, std::string> map, std::string title)
{
	size_t i = 1;
		std::cout << "\n<<<   "<< title << "   >>>" << std::endl;
	for (std::map<int, std::string>::iterator it = map.begin(); it != map.end(); it++)
		{
			std::cout << "--- Item " << i++ << " value: ";
			std::cout << it->first << " " << it->second << "\n";
		}
	std::cout << std::endl;
}

void	print_list_content(std::list<std::string> list, std::string title)
{
	size_t i = 1;
		std::cout << "\n<<<   "<< title << "   >>>" << std::endl;
	for (std::list<std::string>::iterator it = list.begin(); it != list.end(); it++)
		{
			std::cout << "--- Item " << i++ << " value: ";
			std::cout << *it << "\n";
		}
	std::cout << std::endl;
}

void	print_list_content(std::list<int> list, std::string title)
{
	size_t i = 1;
		std::cout << "\n<<<   "<< title << "   >>>" << std::endl;
	for (std::list<int>::iterator it = list.begin(); it != list.end(); it++)
		{
			std::cout << "--- Item " << i++ << "  value: ";
			std::cout << *it << "\n";
		}
	std::cout << std::endl;
}

void	print_vector_content(std::vector<std::string> vector, std::string title)
{
	size_t i = 1;
		std::cout << "\n<<<   "<< title << "   >>>" << std::endl;
	for (std::vector<std::string>::iterator it = vector.begin(); it != vector.end(); it++)
		{
			std::cout << "--- Item " << i++ << "  value: ";
			std::cout << *it << "\n";
		}
	std::cout << std::endl;
}