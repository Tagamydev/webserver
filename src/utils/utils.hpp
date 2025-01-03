#pragma once
#ifndef	UTILS_HPP
# define UTILS_HPP
# include "main.hpp"

class	utils
{
	public:
		~utils();

		// funtions
		static struct pollfd pollfd_from_fd(int fd, short events);
		static void send_response(int socket_fd, const std::string &response_str);
		static std::string	read_file(int fd);

		static void    ft_toLower(std::string &str);
		static void	trim_space_newline(std::string &line);
		static void	trim_semicolon(std::string &line);
		static void	trim_curly_brackets(std::string &line);

		static int		is_empty(std::string &line);
		static void	fix_spaces_in_line(std::string &line);
		static void	print_vector_content(std::vector<std::string> vector, std::string title);
		static void	print_vector_content(std::vector<int> vector, std::string title);
		static void	print_list_content(std::list<std::string> list, std::string title);
		static void	print_list_content(std::list<int> list, std::string title);
		static void	print_map_content(std::map<int, std::string> map, std::string title);

		static std::vector<std::string>	split_to_vector(std::string line, char sep);
		static std::list<std::string>	split_to_list(std::string line, char sep);

		static void map_to_vector_populate_keys(const std::map<std::string, location>& inputMap, std::vector<std::string>& outputVector);
		static void map_to_vector_populate_values(const std::map<std::string, std::string>& inputMap, std::vector<std::string>& outputVector);
		
	private:
		utils();

};

#endif
