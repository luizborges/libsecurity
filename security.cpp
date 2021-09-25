#include "security.hpp"

////////////////////////////////////////////////////////////////////////////////
// Init global variables
////////////////////////////////////////////////////////////////////////////////
std::string security::database_connection = "";
std::string security::system_name = "";


////////////////////////////////////////////////////////////////////////////////
// Functions
////////////////////////////////////////////////////////////////////////////////
void security::config(const std::string& system, const std::string& database_connection) 
{
 try {
    if(system.empty()) throw err("system name string is empty");
    if(database_connection.empty()) throw err("database connection string is empty");
	
	security::system_name = system;
	security::database_connection = database_connection;
 } catch (const std::exception &e) { throw err(e.what()); }
}



bool security::has(const std::string& user, const std::string& role,
				   const std::string& system, const std::string& database_connection)
{
 try {
    if(system.empty()) throw err("system name string is empty");
    if(database_connection.empty()) throw err("database connection string is empty");
    
    pqxx::connection C {database_connection};
	try { // necessário for run C.disconnect() in catch()
    	if(!C.is_open()) throw err("Can't open session database.\nconnection_arg: \"%s\"", database_connection.c_str());
	    pqxx::nontransaction N {C};// inicia uma transação com o BD - Create a notransactional object.
    
        ////////////////////////////////////////////////////////////////////////////////
		// executa o sql e insere o resultado nas estruturas
		////////////////////////////////////////////////////////////////////////////////
		const std::string sql = 
		"SELECT EXISTS( "
			"SELECT 1 "
				"FROM users "
					"INNER JOIN users_set ON users.id = users_set.user_id "
					"INNER JOIN users_set_head ON users_set_head.id = users_set.set_id "
					"INNER JOIN binding_users_role ON binding_users_role.users_set_id = users_set_head.id "
					"INNER JOIN role_set_head ON role_set_head.id = binding_users_role.role_set_id "
					"INNER JOIN role_set ON role_set.set_id = binding_users_role.role_set_id  "
					"INNER JOIN role ON role.id = role_set.role_id "
					"INNER JOIN system ON system.id = role_set_head.system_id "
		
				"WHERE "
					"users.email = "+N.quote(user)+" AND "
					"role.name = "+N.quote(role)+" AND "
					"system.name = "+N.quote(system)+" "
	
				"LIMIT 1 "
		");";
		
		
		////////////////////////////////////////////////////////////////////////////////
		// executa o sql e recebe o valor
		////////////////////////////////////////////////////////////////////////////////
    	auto R{ N.exec1(sql) }; // executa o sql
    	bool _has = R[0].is_null() ? false : R[0].as<bool>(); // recupera o valor retornado pelo sql
    	C.disconnect ();
		return _has;
 	} catch (pqxx::sql_error const &e) {
 		C.disconnect(); throw err("SQL error: %s\nQuery was: \"%s\"", e.what(), e.query().c_str());
	} catch(std::exception const& e) { C.disconnect(); throw err(e.what()); }
 } catch (const std::exception &e) { throw err(e.what()); }
}

void security::check(const std::string& user, const std::string& role,
					 const std::string& system, const std::string& database_connection)
{
 try {
	bool has_ = has(user, role, system, database_connection);
	#if defined(__GNUC__) || defined(__clang__)
		if(!has_) throw no_user_system_role(user, role, system, __FILE__, __PRETTY_FUNCTION__, __LINE__);
	#elif defined(_MSC_VER) // compilador da microsoft 
		if(!has_) throw no_user_system_role(user, role, system, __FILE__, __FUNCSIG__, __LINE__);
	#else // qualquer outro compilador
		if(!has_) throw no_user_system_role(user, role, system, __FILE__, __func__, __LINE__);
	#endif // else -> compiler option
 } catch (const no_user_system_role &e) { throw e;
 } catch (const std::exception &e) { throw err(e.what()); }
}



////////////////////////////////////////////////////////////////////////////////
// Useful SQL
////////////////////////////////////////////////////////////////////////////////
/*
 * FUNC: CHECK()
 * check if the role exists in database
 * SELECT EXISTS(
	SELECT 1
		FROM users 
			INNER JOIN users_set ON users.id = users_set.user_id
			INNER JOIN users_set_head ON users_set_head.id = users_set.set_id
			INNER JOIN binding_users_role ON binding_users_role.users_set_id = users_set_head.id
			INNER JOIN role_set_head ON role_set_head.id = binding_users_role.role_set_id
			INNER JOIN role_set ON role_set.set_id = binding_users_role.role_set_id 
			INNER JOIN role ON role.id = role_set.role_id
			INNER JOIN system ON system.id = role_set_head.system_id
		
		WHERE
			users.email = 'luizbborges@gmail.com' AND
			role.name = 'person_add' AND
			system.name = 'pet'
	
		LIMIT 1
);
 */

