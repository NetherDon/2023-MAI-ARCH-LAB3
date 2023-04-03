#include "cart.h"
#include "database.h"
#include "catalog.h"
#include "../config/config.h"

#include <Poco/Data/MySQL/Connector.h>
#include <Poco/Data/MySQL/MySQLException.h>
#include <Poco/Data/SessionFactory.h>
#include <Poco/Data/RecordSet.h>
#include <Poco/JSON/Parser.h>
#include <Poco/Dynamic/Var.h>

#include <sstream>
#include <exception>

using namespace Poco::Data::Keywords;
using Poco::Data::Session;
using Poco::Data::Statement;

namespace database
{
    void Cart::init()
    {
        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            Statement create_stmt(session);
            create_stmt << "CREATE TABLE IF NOT EXISTS `Cart` (`id` INT NOT NULL AUTO_INCREMENT,"
                        << "`productId` INT NOT NULL,"
                        << "`userId` INT NOT NULL,"
                        << "PRIMARY KEY (`id`),"
                        << "FOREIGN KEY (`productId`) REFERENCES Product (`id`),"
                        << "FOREIGN KEY (`userId`) REFERENCES User (`id`));",
                now;
        }
        catch (Poco::Data::MySQL::ConnectionException &e)
        {
            std::cout << "connection:" << e.what() << std::endl;
            throw;
        }
        catch (Poco::Data::MySQL::StatementException &e)
        {

            std::cout << "statement:" << e.what() << std::endl;
            throw;
        }
    }

    std::vector<Product> Cart::readAllFor(const long &id)
    {
        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            Statement select(session);
            std::vector<Product> result;
            Product a;

            std::string sql = "SELECT id, name, type, description, manufactorer, price FROM Product WHERE id IN (SELECT productId FROM Cart WHERE userId=" + std::to_string(id) + ")";
            std::cout << "SQL Request: " << sql << std::endl;
            select << sql,
                into(a.id()),
                into(a.name()),
                into(a.type()),
                into(a.description()),
                into(a.manufactorer()),
                into(a.price()),
                range(0, 1);

            while (!select.done())
            {
                if (select.execute())
                    result.push_back(a);
            }
            return result;
        }
        catch (Poco::Data::MySQL::ConnectionException &e)
        {
            std::cout << "connection:" << e.what() << std::endl;
            throw;
        }
        catch (Poco::Data::MySQL::StatementException &e)
        {

            std::cout << "statement:" << e.what() << std::endl << e.displayText() << std::endl;
            throw;
        }
    }

    std::vector<Cart> Cart::readAll()
    {
        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            Statement select(session);
            std::vector<Cart> result;
            Cart a;

            std::string sql = "SELECT productId, userId FROM Cart";
            std::cout << "SQL Request: " << sql << std::endl;
            select << sql,
                into(a._product_id),
                into(a._user_id),
                range(0, 1);

            while (!select.done())
            {
                if (select.execute())
                    result.push_back(a);
            }
            return result;
        }
        catch (Poco::Data::MySQL::ConnectionException &e)
        {
            std::cout << "connection:" << e.what() << std::endl;
            throw;
        }
        catch (Poco::Data::MySQL::StatementException &e)
        {

            std::cout << "statement:" << e.what() << std::endl << e.displayText() << std::endl;
            throw;
        }
    }

    void Cart::saveToMySQL()
    {
        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            Poco::Data::Statement insert(session);

            insert << "INSERT INTO Cart (productId, userId) VALUES(?, ?)",
                use(_product_id),
                use(_user_id);

            insert.execute();

            Poco::Data::Statement select(session);
            select << "SELECT LAST_INSERT_ID()",
                into(_id),
                range(0, 1); //  iterate over result set one row at a time

            if (!select.done())
            {
                select.execute();
            }
            std::cout << "Inserted cart id: " << _id << std::endl;
        }
        catch (Poco::Data::MySQL::ConnectionException &e)
        {
            std::cout << "connection:" << e.what() << std::endl;
            throw;
        }
        catch (Poco::Data::MySQL::StatementException &e)
        {

            std::cout << "statement:" << e.what() << std::endl;
            throw;
        }
    }

    Poco::JSON::Object::Ptr Cart::toJSON(bool exclude_id=false) const
    {
        Poco::JSON::Object::Ptr root = new Poco::JSON::Object();

        if (!exclude_id)
            root->set("id", _id);
        root->set("productId", _product_id);
        root->set("userId", _user_id);

        return root;
    }

    long Cart::getId() const { return _id; }
    const long &Cart::getProductId() const { return _product_id; }
    const long &Cart::getUser() const { return _user_id; }

    long& Cart::id() { return _id; }
    long &Cart::product() { return _product_id; }
    long &Cart::user() { return _user_id; }
}