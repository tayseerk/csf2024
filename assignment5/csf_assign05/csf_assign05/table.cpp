#include <cassert>
#include "table.h"
#include "exceptions.h"
#include "guard.h"

Table::Table( const std::string &name )
  : m_name( name )
  // TODO: initialize additional member variables
{
  // TODO: implement
  pthread_mutex_init(&mutex, NULL);
}

Table::~Table()
{
  // TODO: implement
  pthread_mutex_destroy(&mutex);
}

void Table::lock()
{
  // TODO: implement
  pthread_mutex_lock(&mutex);
}

void Table::unlock()
{
  // TODO: implement
  pthread_mutex_unlock(&mutex);
}

bool Table::trylock()
{
  // TODO: implement
  return pthread_mutex_trylock(&mutex) == 0;
}

void Table::set( const std::string &key, const std::string &value )
{
  // TODO: implement
  if(has_key(key)){
    save_original[key] = table[key]; //save original key-value pair in separate map
  } else {
    added_keys.push_back(key); // remember keys that were added in separate vector
  }
  table[key] = value;

}

std::string Table::get( const std::string &key )
{
  // TODO: implement
  return table[key];
}

bool Table::has_key( const std::string &key )
{
  // TODO: implement
  return table.find(key) != table.end();
}

void Table::commit_changes()
{
  // TODO: implement
  save_original.clear(); // do not need original values anymore
  added_keys.clear(); // clear vector of added keys

}

void Table::rollback_changes()
{
  // TODO: implement
  std::map<std::string, std::string>::iterator itr = save_original.begin();

  // for the map with original values
  while(itr != save_original.end()){
    std::string key = itr->first; // get key
    table[key] = save_original[key]; // change table to original values
    itr++;
  }

  for (const auto& key : added_keys) {
      table.erase(key); // erase all added_keys
  }
  added_keys.clear();



}
