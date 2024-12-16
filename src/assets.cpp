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

/*
@brief transform a string in lower case
 */
void    ft_toLower(std::string &str)
{
    for (size_t i = 0; i < str.length(); ++i)
        str[i] = std::tolower(str[i]);
}

/*
@brief trim al spaces and new lines from the start and the end on a string
 */
void trim_space_newline(std::string &line)
{
	int i = 0;
	int j = line.length() - 1;

	while (line[i] != '\0' && line[i] == ' ' || line[i] == '\t' || line[i] == '\r' || line[i] == '\f' || line[i] == '\n')
		i++;
	while (line[j] != '\0' && line[j] == ' ' || line[j] == '\t' || line[j] == '\r' || line[j] == '\f' || line[j] == '\n')
		j--;
	line = line.substr(i,j - i +1);
}

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