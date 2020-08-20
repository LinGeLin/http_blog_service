#define MYSQLPP_MYSQL_HEADERS_BURIED
#include "httplib.h"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include <mysql++/mysql++.h>
#include <iostream>
#include <string>
#include "blog_info.h"
using namespace std;

int main() {
  using namespace httplib;
  using namespace mysqlpp;
  using namespace rapidjson;

  mysqlpp::Connection conn(false);
  conn.set_option(new mysqlpp::SetCharsetNameOption("utf8"));
  if (conn.connect(k_blog_db.c_str(), k_blog_server.c_str(), k_blog_user.c_str(),
                      k_blog_pass.c_str(), k_blog_port)) {
    cout << "success" << endl;
  } else {
    exit(0);
  }

  Server svr;
  svr.Get("/", [](const Request& req, Response& res) {
    cout << "/" << endl;
    res.set_content("hello world!", "text/plain");
  });

  // 增加博客
  svr.Post("/blog", [&](const Request& req, Response& res) {
    Document d;
    if (d.Parse(req.body.c_str()).HasParseError()) {
      return 1;
    }

    auto title = d["title"].GetString();
    auto content = d["content"].GetString();
    auto tag_id = d["tag_id"].GetInt();
    auto create_time = d["create_time"].GetString();
    unique_ptr<char> sql(new char[strlen(content) * 2 + 4096]);
    sprintf(sql.get(), "insert into blog_table values(null, '%s', '%s', '%d', '%s')",
            title, content, tag_id, create_time);
    mysqlpp::Query query = conn.query(string(sql.get()));
    Document d_res;
    d_res.SetObject();

    rapidjson::StringBuffer s;
    rapidjson::Writer<rapidjson::StringBuffer> writer(s);
    try {
      query.execute();
      d_res.AddMember("ok", true, d_res.GetAllocator());
      d_res.Accept(writer);
      res.set_content(string(s.GetString()),"application/json");
    } catch(const mysqlpp::BadQuery& er) {
      d_res.AddMember("ok", false, d_res.GetAllocator());
      d_res.Accept(writer);
      res.set_content(string(s.GetString()), "application/json");
      return -1;
    }
  });

  // 删除博客
  svr.Delete("/blog", [&](const Request& req, Response& res){
    Document d_res;
    d_res.SetObject();
    rapidjson::StringBuffer s;
    rapidjson::Writer<rapidjson::StringBuffer> writer(s);
    if (req.has_param("blog_id")) {
      string blog_id = req.get_param_value("blog_id");
      unique_ptr<char> sql(new char[blog_id.size() * 2 + 4096]);
      sprintf(sql.get(), "delete from blog_table where blog_id=%s", blog_id.c_str()); 

      mysqlpp::Query query = conn.query(string(sql.get()));
      try {
        query.execute();
        d_res.AddMember("ok", true, d_res.GetAllocator());
        d_res.Accept(writer);
        res.set_content(string(s.GetString()),"application/json");
      } catch(const mysqlpp::BadQuery& er) {
        d_res.AddMember("ok", false, d_res.GetAllocator());
        d_res.Accept(writer);
        res.set_content(string(s.GetString()), "application/json");
        return -1;
      }
    } else {
      cout << "no blog_id" << endl;
      d_res.AddMember("ok", false, d_res.GetAllocator());
      d_res.Accept(writer);
      res.set_content(string(s.GetString()), "application/json");
      return -1;
    }
  });

  svr.listen("localhost", 1234);
}