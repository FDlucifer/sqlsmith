#ifndef GRAMMAR_HH
#define GRAMMAR_HH

#include "relmodel.hh"

struct prod {
  virtual std::string str() = 0;
};

struct table_ref : public prod {
  named_relation *t;
  static table_ref *factory(scope &s);
  virtual ~table_ref() { }
};

struct table_or_query_name : public table_ref {
  std::string str();
  table_or_query_name(scope &s);
  virtual ~table_or_query_name() { }
};

struct table_subquery : public table_ref {
  std::string str();
  struct query_spec *query;
  table_subquery(scope &s);
  static int instances;
  virtual ~table_subquery();
};

struct joined_table : table_ref {
  std::string str();  
  joined_table(scope &s);
  std::string type;
  std::string condition;
  table_ref *lhs;
  table_ref *rhs;
  virtual ~joined_table() {
    delete lhs;
    delete rhs;
  }
};

struct from_clause : public prod {
  std::vector<table_ref*> reflist;
  std::string str();
  from_clause(scope &s);
  ~from_clause() { for (auto p : reflist) delete p; }
};

struct table_expression : public prod {
  from_clause fc;
  table_expression(scope &s) : fc(s) { } ; 
  std::string str() {
    return fc.str();
  }
};

struct value_expression: public prod {
  std::string type;
  virtual std::string str() = 0;
  virtual ~value_expression() { }
  static struct value_expression *factory(struct query_spec *q);
};

struct const_expression: value_expression {
  const_expression() { type = "integer"; }
  std::string str() { return std::string("42"); }
  virtual ~const_expression() { }
};

struct column_reference: value_expression {
  column_reference(struct query_spec *q);
  std::string str() { return reference; }
  std::string reference;
  virtual ~column_reference() { }
};

struct select_list : public prod {
  struct query_spec *query;
  std::vector<value_expression*> value_exprs;
  relation derived_table;
  int columns = 0;
  select_list(struct query_spec *q);
  std::string str();
  ~select_list() { for (auto p : value_exprs) delete p; }
};

struct query_spec : public prod {
  std::string set_quantifier;
  table_expression expr;
  select_list sl;
  std::string limit_clause;
  std::string str();
  query_spec(scope &s);
  virtual ~query_spec() { }
};


#endif
