#include <iostream>
#include <pqxx/pqxx>
#include <util.hpp>
#include <security.hpp>
using namespace std;

#define DATABASE_CONNECTION "dbname=pet user=borges password=JSG3bor_g873sqlptgs78b hostaddr=127.0.0.1 port=5432"
#define DATABASE_CONNECTION_SECURITY "dbname=security user=borges password=JSG3bor_g873sqlptgs78b hostaddr=127.0.0.1 port=5432"

/**
 selects:
  - to retry time in minutes in session:
  SELECT EXTRACT(EPOCH FROM (CURRENT_TIMESTAMP - (select date from head where id = '123')))/60;
 */

//void* CB;
pqxx::connection_base* CB;

int main() {
 try {
	 security::config("pet", DATABASE_CONNECTION_SECURITY);
	 if(security::has("luizbborges@gmail.com", "perxson_add")) printf("go 1\n");
	 else printf("not 1\n");
	 
	 try {
		 security::check("luizbborges@gmail.com", "perxson_add");
		 printf("It works!!!\n");
	} catch(const std::exception& e) {
		printf("\"\%s\"\n", e.what());
	}
	 
	 
	 
 } catch (const std::exception &e) {
	err(e.what());
	 
	}
	return 0;
}























