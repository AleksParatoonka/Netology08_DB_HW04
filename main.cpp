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
			std::cout << "Boooo" << ".\n";
			c = new pqxx::connection(
				"host=localhost "
				"port=5432 "
				
				"dbname=client_management_service_01 "
				//"dbname=music_service "
				"user=postgres "
				"password=new_password");
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

		void update_info(int client_id, int info_type, std::string info) {	
			pqxx::work tx{ *c };
			switch (info_type) {
				case 0: //name
					
					tx.exec("UPDATE clients SET name='" + tx.esc(info) + "' WHERE id = '" + std::to_string(client_id) + "'");
					tx.commit();
					break;
				case 1: // surname
					
					tx.exec("UPDATE clients SET surname='" + tx.esc(info) + "' WHERE id = '" + std::to_string(client_id) + "'");
					tx.commit();
					break;
				case 2: //email
					
					tx.exec("UPDATE clients SET email='" + tx.esc(info) + "' WHERE id = '" + std::to_string(client_id) + "'");
					tx.commit();
					break;
			} 			
		}

		void selection(int info_type, std::string info) {
			int client_id = -1;

			pqxx::work tx{ *c };
			
			switch (info_type) {
			case 0: //name
				{
				int it = 0;
				int client_id_tmp = -1;
				auto resultIteration = tx.query<int, std::string, std::string>("SELECT id, surname, email FROM clients WHERE name ='" + tx.esc(info) + "'");

				for (std::tuple<int, std::string, std::string> res : resultIteration)
				{
					std::cout << " name: " << info << " surname: " << std::get<1>(res) << " email: " << std::get<2>(res) << ".\n";
					it++;
					client_id_tmp = std::get<0>(res);
				}
				if (it == 1) {
					client_id = client_id_tmp;
				}
				tx.commit();
				}
				break;

			case 1: // surname
				{
				int it1 = 0;
				int client_id_tmp1 = -1;
				auto resultIteration1 = tx.query<int, std::string, std::string>("SELECT id, name, email FROM clients WHERE surname ='" + tx.esc(info) + "'");

				for (std::tuple<int, std::string, std::string> res : resultIteration1)
				{
					std::cout << " name: " << std::get<1>(res) << " surname: " << info << " email: " << std::get<2>(res) << ".\n";
					it1++;
					client_id_tmp1 = std::get<0>(res);
				}
				if (it1 == 1) {
					client_id = client_id_tmp1;
				}
				tx.commit();
				}
				break;

			case 2: //email
			{
				client_id = tx.query_value<int>("SELECT id FROM clients WHERE email ='" + tx.esc(info) + "'");
				tx.commit();
			}
				break;

			case 3: //phone
			{
				client_id = tx.query_value<int>("SELECT client_id FROM phones WHERE phone ='" + tx.esc(info) + "'");
				tx.commit();
			}
				break;
			}

			selected_client_id = client_id;
		}
		void add_phone(int client_id, std::string phone1) {
			pqxx::work tx{ *c };
			std::cout << "hoooo " << phone1 << "hi " << client_id << std::endl;
			std::cout << "INSERT INTO phones(client_id, phone) VALUES('" + std::to_string(client_id) + "', '" + tx.esc(phone1) + "')" << std::endl;
			tx.exec("INSERT INTO phones(client_id, phone) VALUES('" + std::to_string(client_id) + "', '" + tx.esc(phone1) + "')");
			tx.commit();
		}
		void delete_phone(int client_id1, std::string phone1) {
			pqxx::work tx{ *c };
			tx.exec("DELETE FROM phones WHERE client_id ='" + std::to_string(client_id1) + "' AND phone='" + tx.esc(phone1)+"' ");
			tx.commit();
		}
		void add_client(std::string name1, std::string surname1, std::string email1, std::string phone1 = "") {
			pqxx::work tx{ *c };
			
			tx.exec("INSERT INTO clients(name, surname, email) VALUES('" + tx.esc(name1) + "', '" + surname1 + "', '" + tx.esc(email1) + "')");
			

			tx.commit();
			
			if (phone1 != "") {
				pqxx::work tx1{ *c };
				int client_id = tx1.query_value<int>("SELECT id FROM clients WHERE name = '" + tx.esc(name1) + "' AND surname = '" + tx.esc(surname1) + "' AND email = '" + tx.esc(email1) + "' ");
				tx1.commit();
				
				this->add_phone(client_id, phone1);
			}
		}
		void delete_client(int client_id1) {
			pqxx::work tx{ *c };
			tx.exec("DELETE FROM phones WHERE client_id='" + std::to_string(client_id1) + "'");
			tx.exec("DELETE FROM clients WHERE id='" + std::to_string(client_id1) + "'");
			tx.commit();
		}
		
};


int main(){
	SetConsoleCP(CP_UTF8);
	SetConsoleOutputCP(CP_UTF8);
	try
	{
		ClientsManagement base;
		base.create_table();

		base.add_client("May", "Morrow", "nooaaa@bk.ru");
		base.add_phone(1 , "89272602604");
		//base.add_client("May", "Morrowrr", "jjj@bk.ru", "89272560604");
		//base.add_phone(2, "89172360687");
		/*base.delete_client(1);
		base.delete_phone(2, "89272560604");
		base.update_info(2, 0, "Fan");	
		base.add_client("May", "Mack", "nooaa111a@bk.ru");
		base.selection(0, "May");*/
		
	}
	catch (pqxx::sql_error e)
	{
		std::cout << e.what() << std::endl;
	}
}