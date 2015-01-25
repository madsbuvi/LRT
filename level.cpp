#include "level.h"

#include <iostream>
#include <fstream>
#include <cassert>

#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/spirit/include/support_istream_iterator.hpp>
#include <boost/bind.hpp>

namespace parser
{

using std::ofstream;
void save( const char* filename, RTContext& context )
{
	ofstream out;
	out.open( filename );
	out << context;
	out.close();
}

namespace fusion = boost::fusion;
namespace phoenix = boost::phoenix;
namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;

static void make_float3 ( float3& val, float a1, float a2, float a3 )
{
	val = ::make_float3( a1, a2, a3 );
}

static void make_aab ( std::vector<Geometry*>& geometry, std::vector<Shader*>& shaders, int shader, float3 a1, float3 a2 )
{
	Geometry* geo = new Geometry_AAB( a1, a2 );
	geo->setShader( shaders[shader] );
	geometry.push_back( geo );
}

static void make_aap ( std::vector<Geometry*>& geometry, std::vector<Shader*>& shaders, int shader, float3 a1, int orientation )
{
	Geometry* geo = new Geometry_AAP( a1, Orientation(orientation) );
	geo->setShader( shaders[shader] );
	geometry.push_back( geo );
	
}

static void make_simpleDiffusionTex ( std::vector<Geometry*>& geometry, std::vector<Shader*>& shaders, int ID, std::string filename )
{
	assert( ID >= 0 );
	assert( unsigned(ID) == shaders.size() );
	shaders.push_back( new SimpleDiffusionShaderTex( filename.c_str() ) );
	std::cout << ID << ": " << filename << std::endl;
}

using boost::spirit::qi::parse;

template< typename Iterator >
struct LevelGrammar : qi::grammar< Iterator, qi::unused_type, ascii::space_type >
{
	
	LevelGrammar() : LevelGrammar::base_type( r_top )
	{
		r_float3 = ("FLOAT3{" >> qi::double_ >> -boost::spirit::lit(",") >> qi::double_>> -boost::spirit::lit(",") >> qi::double_ >>"}")
			[ boost::phoenix::bind( &make_float3, qi::_val, qi::_1, qi::_2, qi::_3 ) ];
		r_enum = ("ENUM{" >> qi::int_ >> "}");
		r_string = qi::lit("\"") >> +(ascii::char_ - '"') >> qi::lit("\"");
		
		r_aab = ( boost::spirit::lit("AXISALIGNEDBOX") >> "{" >> qi::int_ >> "}" >> "{" >> r_float3 >> r_float3 >> "}")
			[ boost::phoenix::bind( &make_aab,
									boost::phoenix::ref(geometry), 
									boost::phoenix::ref(shaders), 
									qi::_1, qi::_2, qi::_3 ) ];
		r_aap = ( boost::spirit::lit("AXISALIGNEDPRISM") >> "{" >> qi::int_ >> "}" >> "{" >> r_float3 >> r_enum >> "}")
			[ boost::phoenix::bind( &make_aap,
									boost::phoenix::ref(geometry), 
									boost::phoenix::ref(shaders), 
									qi::_1, qi::_2, qi::_3 ) ];
		r_geometry = r_aab | r_aap;
		
		r_simpleDiffusionTex = ( boost::spirit::lit("SIMPLEDIFFUSIONSHADERTEX") >> "{" >> qi::int_ >> "}" >> "{" >> r_string >> "}")
			[ boost::phoenix::bind( &make_simpleDiffusionTex,
									boost::phoenix::ref(geometry), 
									boost::phoenix::ref(shaders), 
									qi::_1, qi::_2 ) ];
		r_shader = r_simpleDiffusionTex;
		
		r_geometrylist = "GEOMETRYLIST{" >> *r_geometry >> "}";
		r_shaderlist = "SHADERLIST{" >> *r_shader >> "}";
		r_top = r_shaderlist >> r_geometrylist;
	}
	
	qi::rule< Iterator, float3(), ascii::space_type >			r_float3;
	qi::rule< Iterator, int(), ascii::space_type >				r_enum;
	qi::rule< Iterator, std::string(), ascii::space_type >		r_string;
	
	qi::rule< Iterator, qi::unused_type, ascii::space_type >	r_aab;
	qi::rule< Iterator, qi::unused_type, ascii::space_type >	r_aap;
	qi::rule< Iterator, qi::unused_type, ascii::space_type >	r_geometry;
	
	qi::rule< Iterator, qi::unused_type, ascii::space_type >	r_simpleDiffusionTex;
	qi::rule< Iterator, qi::unused_type, ascii::space_type >	r_shader;
	
	qi::rule< Iterator, qi::unused_type, ascii::space_type >	r_geometrylist;
	qi::rule< Iterator, qi::unused_type, ascii::space_type >	r_shaderlist;
	qi::rule< Iterator, qi::unused_type, ascii::space_type >	r_top;
	
	public:
	std::vector<Geometry*> geometry;
	std::vector<Shader*> shaders;
};

//SIMPLEDIFFUSIONSHADERTEX{4}{ "Textures/Brown-L3D.png" }
//AXISALIGNEDBOX{FLOAT3{1,7,3}FLOAT3{2,8,4}}
//AXISALIGNEDPRISM{ FLOAT3{1,6,2} ENUM{ 9 } }

void load( const char* filename, RTContext& context )
{
    using boost::spirit::ascii::space;

	std::ifstream in( filename );
	if( !in.good() )
	{
		std::cerr << "Could not open file " << std::string(filename) << " for reading\n" << std::endl;
	}

	in.unsetf(std::ios::skipws);
	std::string storage;
	std::copy(
		std::istream_iterator<char>(in),
		std::istream_iterator<char>(),
		std::back_inserter(storage)
	);

	LevelGrammar<std::string::const_iterator> test;
	std::string::const_iterator iter = storage.begin();
	std::string::const_iterator end = storage.end();
	
	bool r = phrase_parse( iter, end, test, space );
    if (r && iter == end)
    {
        std::cout << "-------------------------\n";
        std::cout << "Parsing succeeded:\n";
		std::cout << test.geometry.size() << "\n";
		for( Geometry* boop: test.geometry)
		{
			boop->writeOff(std::cout);
			context.addGeometry(boop);
			std::cout << "\n";
		}
        std::cout << "-------------------------\n";
    }
    else
    {
        std::string::const_iterator some = iter+30;
        std::string context(iter, (some>end)?end:some);
        std::cout << "-------------------------\n";
        std::cout << "Parsing failed\n";
        std::cout << "stopped at: \"" << context << "...\"\n";
        std::cout << "-------------------------\n";
    }
	
	in.close();
}
} // namespace parser