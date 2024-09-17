/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClassPref.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: samusanc <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/08 19:56:16 by samusanc          #+#    #+#             */
/*   Updated: 2024/06/04 21:52:22 by samusanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef	CLASS_HPP
# define CLASS_HPP
# include <string>
# include <iostream>

class	Class{
	/* here every public statement */
	public:
		/* Constructor */
		Class(void);
		/* Copy Constructor */
		Class(const Class& other);
		/* Copy assignment operator overload */
		Class& operator = (const Class& other);
		/* Destructor */
		~Class(void);

	/* here every private statement */
	private:
};

#endif
