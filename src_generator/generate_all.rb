#!/usr/bin/env ruby

require 'pp'
require 'erubis'
require 'yaml'
require 'fileutils'
require "./serialize.rb"

data = YAML.load_file("./signalmap.yaml")

###########################################################################
###########################################################################
###########################################################################
###########################################################################

def generate_c_header( tag, data )
  template = '''
/*\
  Automatically generated
\*/

#ifndef <%= @tag.upcase %>_H
#define <%= @tag.upcase %>_H

#ifdef __cplusplus
  extern "C" {
#endif

/* C structure for <%= @tag %> */
<%= to_C_struct( @tag, @value ) %>

/* print <%= @tag %> struct on stdio */
extern
void
buffer_to_<%= @tag %>( uint8_t const buffer[], <%= @tag %> * S );

/* serialize <%= @tag %> struct to buffer */
extern
void
<%= @tag %>_to_buffer( <%= @tag %> const * S, uint8_t buffer[] );

/* get buffer and un-serialize to <%= @tag %> struct */
extern
void
<%= @tag %>_print( <%= @tag %> const * S );

/* build topic for <%= @tag %> struct */
extern
void
<%= @tag %>_MQTT_topic( <%= @tag %> const * S, char topic[], int topic_len );

/* check topic for <%= @tag %> struct */
extern
int
<%= @tag %>_MQTT_compare( char const topic[] );

/* build topic for <%= @tag %> struct */
extern
void
<%= @tag %>_MQTT_alltopics( char topic[], int topic_len );

#ifdef __cplusplus
};
#endif

#endif
'''
  context = { :tag => tag, :value => data[tag] }
  eruby = Erubis::Eruby.new(template)
  eruby.evaluate(context)
end

###########################################################################
###########################################################################
###########################################################################
###########################################################################

def generate_c_body( tag, data )
  template = '''
/*\
  Automatically generated
\*/

#include "buffer_defines.h"
#include "<%= @tag %>.h"
#include <stdio.h>
#include <string.h>

#ifdef __cplusplus
  extern "C" {
#endif

/* print <%= @tag %> struct on stdio */
<%= to_print( @tag, @value ) %>

/* serialize <%= @tag %> struct to buffer */
<%= to_buffer( @tag, @value ) %>

/* get buffer and un-serialize to <%= @tag %> struct */
<%= from_buffer( @tag, @value ) %>

/* build topic for <%= @tag %> struct */
<%= to_MQTT_topic( @tag, @data ) %>

/* build topic for <%= @tag %> struct */
<%= to_MQTT_alltopics( @tag, @data ) %>


#ifdef __cplusplus
};
#endif

'''
  context = { :tag => tag, :value => data[tag], :data => data }
  eruby = Erubis::Eruby.new(template)
  eruby.evaluate(context)  
end

#  ____
# / ___| _     _
# | |   _| |_ _| |_
# | |__|_   _|_   _|
# \____||_|   |_|
#

###########################################################################
###########################################################################
###########################################################################
###########################################################################

def generate_cpp_header( data )
  template = '''
/*\
  Automatically generated
\*/

#ifndef <%= @main_topic.upcase %>_HH
#define <%= @main_topic.upcase %>_HH

#include "buffer_defines.h"
#include <mosquitto.h>
#include <mosquittopp.h>
#include <string>
#include <iostream>

<% @data.keys.each do |tag|
  if tag != :origin_file and tag != :main_topic then %>
#include "<%= tag %>.h"
<% end; end; %>

class MQTT_<%= @main_topic %>_publisher : public mosqpp::mosquittopp {
private:
  std::string host;
  std::string id;
  std::string topic;
  int         port;
  int         keepalive;
  int         qos;

  virtual void on_connect(int rc);
  virtual void on_disconnect(int rc);
  virtual void on_publish(int mid);

public:

  MQTT_<%= @main_topic %>_publisher(
    std::string const & id,
    std::string const & topic,
    std::string const & host,
    int                 port
  );

  virtual ~MQTT_<%= @main_topic %>_publisher();

<% @data.keys.each do |tag|
    if tag != :origin_file and tag != :main_topic then %>
  bool publish( <%= tag %> const & S, int * mid = nullptr );
<% end; end; %>
};


class MQTT_<%= @main_topic %>_subscriber : public mosqpp::mosquittopp {
<% @data.keys.each do |tag| if tag != :origin_file and tag != :main_topic then %>
  <%= tag %> <%= tag %>_data;
<% end; end; %>
public:
  MQTT_<%= @main_topic %>_subscriber( char const id[], bool clean_session )
  : mosqpp::mosquittopp( id, clean_session )
  {}

  virtual
  void
  on_connect( int result );

  virtual
  void
  on_message( const struct mosquitto_message *message );
};

#endif

// EOF: <%= @main_topic %>.hpp
'''
  context = { :data => data, :main_topic => data[:main_topic] }
  eruby = Erubis::Eruby.new(template)
  eruby.evaluate(context)
end

###########################################################################
###########################################################################
###########################################################################
###########################################################################

def generate_cpp_body( data )
  template = '''
/*\
  Automatically generated
\*/

#include <iostream>
#include "<%= @main_topic %>.hpp"

MQTT_<%= @main_topic %>_publisher::MQTT_<%= @main_topic %>_publisher(
  std::string const & _id,
  std::string const & _topic,
  std::string const & _host,
  int                 _port
)
: host(_host)
, id(_id)
, topic(_topic)
, port(_port)
, keepalive(false)
, qos(0)
{


}


MQTT_<%= @main_topic %>_publisher::~MQTT_<%= @main_topic %>_publisher() {

}

// on_connect is called by thread each time we exeperience a server connection

void
MQTT_<%= @main_topic %>_publisher::on_connect( int rc ) {

}

// on_disconnect is called by thread each time we exeperience a server disconnection

void
MQTT_<%= @main_topic %>_publisher::on_disconnect( int rc ) {

}

// on_publish is called each time a message succeed to be sent to broker.
// The parameter is the message id you can set when publish.
void
MQTT_<%= @main_topic %>_publisher::on_publish( int mid ) {

}

<% @data.keys.each do |tag|
  if tag != :origin_file and tag != :main_topic then %>

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

bool
MQTT_<%= @main_topic %>_publisher::publish(
  <%= tag %> const & S, int * mid
) {
  char    topic[1000];
  uint8_t buffer[<%= tag %>_size];
  <%= tag %>_MQTT_topic( &S, topic, 1000 );
  <%= tag %>_to_buffer( &S, buffer );
  int ret = this->mosqpp::mosquittopp::publish(
    mid,
    topic,
    <%= tag %>_size,
    buffer,
    this->qos,
    false
  );
  return ret == MOSQ_ERR_SUCCESS;
}

<% end; end %>

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void
MQTT_<%= @main_topic %>_subscriber::on_connect( int result ) {
  if (!result) {
    char topic[1000];
    <% @data.keys.each do |tag| if tag != :origin_file and tag != :main_topic then %>
    <%= tag %>_MQTT_alltopics( topic, 1000 );
    this->subscribe( nullptr, topic );
    <% end; end; %>
  } else {
    std::cerr << "Connect failed\n";
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void
MQTT_<%= @main_topic %>_subscriber::on_message(
  const struct mosquitto_message *message
) {
  uint8_t * ptr = static_cast<uint8_t *>(message->payload);
  if (!message->payloadlen) {
    std::cerr << "Skipping empty payload!\n";
<% @data.keys.each do |tag| if tag != :origin_file and tag != :main_topic then %>
  } else if ( <%= tag %>_MQTT_compare( message->topic ) == 0 ) {
    buffer_to_<%= tag %>( ptr, &<%= tag %>_data );
<% end; end; %>
  } else {
    std::cerr << "unmanaged topic " << message->topic << \'\n\';
  }
}

// EOF: <%= @main_topic %>.cpp
'''
  context = { :data => data, :main_topic => data[:main_topic] }
  eruby = Erubis::Eruby.new(template)
  eruby.evaluate(context)
end

sep = "\n\n\n\n\n"

FileUtils.mkdir_p "./generated"
FileUtils.cp "../src/buffer_defines.h",      "./generated"
FileUtils.cp "../src/buffer_defines_hton.c", "./generated"
FileUtils.cp "../src/buffer_defines_ntoh.c", "./generated"

data.keys.each do |tag|
  if tag != :origin_file and tag != :main_topic then
    value = data[tag];
    ## puts to_print( tag, value )+sep
    ## puts to_buffer( tag, value )+sep
    ## puts from_buffer( tag, value )+sep
    ## puts to_C_struct( tag, value )+sep
    ## puts to_MATLAB_struct( tag, value )+sep
    ## puts to_SIMULINK_struct( tag, value )+sep
    ## puts to_SIMULINK_busInfo( tag, value )+sep
    ## puts to_SIMULINK_busInfo_in_data( tag, value )+sep
    ## puts to_SIMULINK_busInfo_in_data_rtw( tag, value )+sep
    ## puts to_SIMULINK_message( tag, value )+sep
    ## puts simulink_to_buffer( tag, value )+sep
    ## puts simulink_from_buffer( tag, value )+sep
    ## puts simulink_set_output_signal( tag, value )+sep
    ## puts simulink_set_input_signal( tag, value )+sep
    ## puts to_MQTT_topic( tag, data )+sep

    prefix = "generated/"+tag.to_s

    File.open( prefix+".h",   "w" ) { |f| f.puts generate_c_header( tag, data )   }
    File.open( prefix+".c",   "w" ) { |f| f.puts generate_c_body( tag, data )     }
  end
end

prefix = "generated/"+data[:main_topic]

File.open( prefix+".hpp",   "w" ) { |f| f.puts generate_cpp_header( data ) }
File.open( prefix+".cpp",   "w" ) { |f| f.puts generate_cpp_body( data ) }
