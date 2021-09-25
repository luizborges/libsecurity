/**
 *
 * @descripion: funciona como um header que contém todas as bibliotecas, é necessário apenas adicionar
 * essa biblioteca para ter acesso a todas a biblioteca.
 */
#ifndef SECURITY_HPP
#define SECURITY_HPP


////////////////////////////////////////////////////////////////////////////////
// Includes - default libraries - C
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Includes - default libraries - C++
////////////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <cstdio>
#include <cstring>
#include <string>
#include <ctime>
#include <ctype.h>
#include <map> 
#include <deque>
#include <memory>
#include <unordered_map>
#include <fstream>
#include <streambuf>
#include <exception>
#include <list>

#include <pqxx/pqxx> // postgres
////////////////////////////////////////////////////////////////////////////////
// Includes - system dependent libraries
////////////////////////////////////////////////////////////////////////////////
#if defined(unix) || defined(__unix) || defined(__unix__) || (defined (__APPLE__) && defined (__MACH__)) // Unix (Linux, *BSD, Mac OS X)
#include <unistd.h> // unix standard library
#include <sys/syscall.h>
#include <sys/types.h>
#include <dirent.h>
#endif

////////////////////////////////////////////////////////////////////////////////
// Includes - my libraries
////////////////////////////////////////////////////////////////////////////////
#include <util.hpp>

////////////////////////////////////////////////////////////////////////////////
// Includes - namespace
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// namespace
////////////////////////////////////////////////////////////////////////////////
namespace security
{
	////////////////////////////////////////////////////////////////////////////////
	// global variables - init in security.cpp
	////////////////////////////////////////////////////////////////////////////////
	extern std::string database_connection;
	extern std::string system_name;
	
	////////////////////////////////////////////////////////////////////////////////
	// class error - to throw exception
	////////////////////////////////////////////////////////////////////////////////
	
	
	
	////////////////////////////////////////////////////////////////////////////////
	// global functions - file: security.cpp
	////////////////////////////////////////////////////////////////////////////////
	/**
	 * Configura as variáveis globais da biblioteca.
	 * @obs: se qualquer das strings passadas for vazia, é retornado uma exceção
	 * @arg system: seta a variável global system_name
	 * @arg system: seta a variável global database_connection
	 */
	void config(const std::string& system, const std::string& database_connection);
	
	/**
	 * Chama a função has(). Caso ela retorne false, esta função retorna uma exceção.
	 * @obs: em caso de erro é lançado uma exceção
	 * @throw no_user_system_role: exceção que é lançado caso a função has() return false.
	 * @obs: caso ocorra algum erro de execução, que não seja a has() return false, é lançado a exceção u::error
	 */
	void check(const std::string& user, const std::string& role,
			   const std::string& system = security::system_name, 
			   const std::string& database_connection = security::database_connection);
	
	/**
	 * Função feita para verificar se um dado usuário tem permissão para acessar uma dada função(role) de um sistema.
	 * Verifica se no banco de dados de segurança existe uma relação entre a role passada, o usuário e o sistema.
	 * Ou seja, está função monta um sql, em que o usuário, a role ("função do sistema que que se quer
	 * checar se o usuário tem permissão"), o sistema que a que pertence a role, são inseridos nele.
	 * O sql contém vários inners joins, para verificar se é possível construir uma relação, ou seja, se o sql
	 * retorna algum resultado (true), ou, se não existe uma linha(resultado, relação) entre os valores passados.
	 * @obs: os valores @user, @role e @system são passados antes de inseridos no SQL na função quote() do pqxx.
	 * @arg user: usuário que será inserido na linha do sql: "users.email = "+N.quote(user)
	 * @arg role: role que se quer verificar se o usuário tem permissão: role.name = "+N.quote(role)
	 * @arg system: nome do sistema que se está checando: system.name = "+N.quote(system)
	 * @arg database_connection: string para realizar a conexão com o banco de dados)
	 * @return true: caso haja uma relação no Banco de Dados entre o usuário, role e o sistema.
	 * false, otherwise.
	 */
	bool has(const std::string& user, const std::string& role,
			 const std::string& system = security::system_name, 
			 const std::string& database_connection = security::database_connection);
	
	////////////////////////////////////////////////////////////////////////////////
	// Exception Class
	////////////////////////////////////////////////////////////////////////////////
	class no_user_system_role : public std::exception {
		std::string user;
		std::string role;
		std::string system;
		std::string file;
		std::string func;
		int line = -1;
		
	 public:
		no_user_system_role(const std::string& user, const std::string& role,
							const std::string& system, const std::string& file,
							const std::string& func, const int line) {
			this->user   = user;
			this->role   = role;
			this->system = system;
			this->file   = file;
			this->func   = func;
			this->line  = line;
		}
		
		/**
		 * A forma como a string da mensagem de erro é construída é por causa das funções:
		 * u::error msg e u::error::get_trace()
		 * Isto é feito pois a ideia é uma integração entre ambas as classes e bibliotecas, para que
		 * a interface seja comum.
		 * A última linha da msg: é assim, pois se não a u::error colocará a última linha em bold.
		 * Entretanto, esta última linha é referente a função que é chamada, o que na u::error não é bold.
		 * Foi feito assim para integrar ambas as classes do modo mais simples possível.
		 */
		const char* what() const throw() {
			static std::string msg = "Security Exception:: no user binding for system role.\n<br>"
				"user: \'" + user + "\'\n<br>"
				"role: \'" + role + "\'\n<br>"
				"system: \'" + system + "\'\n<br>"
				"</b>\""+ func +"\" ("+ std::to_string(line) +", \""+ file +"\")<b>";
			
			return msg.c_str();
		}
	};
}

////////////////////////////////////////////////////////////////////////////////
// Implementation of templates and inline functions
////////////////////////////////////////////////////////////////////////////////

#endif // SECURITY_HPP

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// SQL create database security
////////////////////////////////////////////////////////////////////////////////
































