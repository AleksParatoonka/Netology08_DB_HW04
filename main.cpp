#include <iostream>
#include <pqxx/pqxx>
#include "Windows.h"
#include <tuple>
#pragma execution_character_set("utf-8")

class ClientsManagement {
	public:
		pqxx::connection* c;
		std::string name;
		std::string surname;
		std::string email;
		int selected_client_id;
		
		ClientsManagement() {
			c = new pqxx::connection(
				"host=localhost "
				"port=5432 "
				"dbname=client_management_service_01"
				"user=postgres "
				"password=Pomidorka01");
		}

		void create_table() {
			pqxx::work tx{ *c };
			tx.exec("CREATE TABLE IF NOT EXISTS clients ("
				"id SERIAL PRIMARY KEY,"
				"name VARCHAR NOT NULL,"
				"surname VARCHAR NOT NULL,"
				"email VARCHAR NOT NULL); ");
			tx.exec("CREATE TABLE IF NOT EXISTS phones ("
				"id SERIAL PRIMARY KEY,"
				"client_id INTEGER REFERENCES clients(id),"
				"phone VARCHAR(15) ); ");
			tx.commit();
		}

		void update_info(int client_id, std::string name1 = "", std::string surname1 = "", std::string email1 = "") {
		
			
			if (email1 != "") {
				pqxx::work tx{ *c };
				tx.exec("UPDATE clients SET email='" + tx.esc(email1) + "' WHERE id = '" + std::to_string(client_id)+"'");
				tx.commit();
			}
			
			else if (name1 != "") {
				pqxx::work tx{ *c };
				tx.exec("UPDATE clients SET name='" + tx.esc(name1) + "' WHERE id = '" + std::to_string(client_id) + "'");
				tx.commit();
			}
			else if (surname1 != "") {
				pqxx::work tx{ *c };
				tx.exec("UPDATE clients SET surname='" + tx.esc(surname1) + "' WHERE id = '" + std::to_string(client_id) + "'");
				tx.commit();
			}
			else {
				std::cout << "Wrong input" << ".\n";
			}
			
		}

		void selection(std::string name1 = "", std::string surname1 = "", std::string email1 = "", std::string phone1 = "") {
			int client_id = -1;
			pqxx::work tx{ *c };
			if (phone1 != "") {
				client_id = tx.query_value<int>("SELECT client_id FROM phones WHERE phone ='" + tx.esc(phone1) + "'");
			}
			else if(email1 != "") {
				client_id = tx.query_value<int>("SELECT id FROM clients WHERE email ='" + tx.esc(email1) + "'");
			}
			else if (name1 != "" && surname1 != "") {
				int it = 0;
				int client_id_tmp = -1;
				auto resultIteration = tx.query<int, std::string>("SELECT id, email FROM clients WHERE name ='" + tx.esc(name1) + "'");

				for (std::tuple<int, std::string> res : resultIteration)
				{
					std::cout << " name: " << name1 << " surname: " << surname1 << " email: " << email1 << ".\n";
					it++;
					client_id_tmp = std::get<0>(res);
				}
				if (it == 1) {
					client_id = client_id_tmp;
				}
			}
			else if (name1 != "") {
				int it = 0;
				int client_id_tmp = -1;
				auto resultIteration = tx.query<int, std::string, std::string>("SELECT id, surname, email FROM clients WHERE name ='" + tx.esc(name1) + "'");

				for (std::tuple<int, std::string, std::string> res : resultIteration)
				{
					std::cout << " name: " << name1 << " surname: " << std::get<1>(res) << " email: " << email1 << ".\n";
					it++;
					client_id_tmp = std::get<0>(res);
				}
				if (it == 1) {
					client_id = client_id_tmp;
				}
			}
			else if (surname1 != "") {
				int it = 0;
				int client_id_tmp = -1;
				auto resultIteration = tx.query<int, std::string, std::string>("SELECT client_id, name, email FROM clients WHERE surname ='" + tx.esc(surname1) + "'");

				for (std::tuple<int, std::string, std::string> res : resultIteration)
				{
					std::cout << " name: " << std::get<1>(res) << " surname: " << surname1 << " email: " << email1 << ".\n";
					it++;
					client_id_tmp = std::get<0>(res);
				}
				if (it == 1) {
					client_id = client_id_tmp;
				}
			}
			else {
				std::cout << "Wrong input" << ".\n";
			}

			selected_client_id = client_id;
		}
		void add_phone(int client_id, std::string phone1) {
			pqxx::work tx{ *c };
			tx.exec("INSERT INTO phones(client_id, phone) VALUES('" + std::to_string(client_id) + "', '" + tx.esc(phone1) + "')");
			tx.commit();
		}
		void delete_phone(int client_id1, std::string phone1) {
			pqxx::work tx{ *c };
			tx.exec("DELETE FROM phones WHERE client_id ='" + std::to_string(client_id1) + "' AND phone='" + tx.esc(phone1)+"'");
			
		}
		void add_client(std::string name1, std::string surname1, std::string email1, std::string phone1 = "") {
			pqxx::work tx{ *c };
			
			tx.exec("INSERT INTO clients(name, surname, email) VALUES('" + tx.esc(name1) + "', '" + surname1 + "', '" + tx.esc(email1) + "')");
			

			tx.commit();
			
			if (phone1 != "") {
				pqxx::work tx1{ *c };
				int client_id = tx1.query_value<int>("SELECT id FROM clients WHERE name = '" + tx.esc(name1) + "' AND surname = '" + tx.esc(surname1) + "' AND email = '" + tx.esc(email1) + "'");

				add_phone(client_id, phone1);
			}
		}
		void delete_client(int client_id1) {
			pqxx::work tx{ *c };
			tx.exec("DELETE FROM phones WHERE client_id='" + std::to_string(client_id1) + "'");
			tx.exec("DELETE FROM clients WHERE client_id='" + std::to_string(client_id1) + "'");
			tx.commit();
		}
		
};


int main(){
	SetConsoleCP(CP_UTF8);
	SetConsoleOutputCP(CP_UTF8);
	try
	{
		ClientsManagement base;
		//base.create_table();
		base.add_client("May", "Morrow", "nooaaa@bk.ru");
		//base.selection();
		
	}
	catch (pqxx::sql_error e)
	{
		std::cout << e.what() << std::endl;
	}
}