#include "rocketmenu.h"
#include <Rocket/Core.h>
#include <unordered_map>
#include <string>
#define GL_CLAMP_TO_EDGE 0x812F

Rocket::Core::Context* rContext;

std::unordered_map<std::string, LrtClickFunc> listeners;
std::unordered_map<std::string, void* > listener_data;
std::unordered_map<std::string, Rocket::Core::ElementDocument* > documents;

static ShellRenderInterfaceOpenGL opengl_renderer;
static ShellSystemInterface system_interface;
static Rocket::Core::ElementDocument* active_document;

void rocketLoop()
{
	rContext->Update();
	rContext->Render();
}

void registerListener( std::string ID, LrtClickFunc listener, void* data )
{
	listeners[ID] = listener;
	listener_data[ID] = data;
}
void registerListener( char* ID, LrtClickFunc listener, void* data )
{
	registerListener( std::string( ID ), listener, data );
}

class LrtButtonListener : public Rocket::Core::EventListener
{
public:
	
	void ProcessEvent(Rocket::Core::Event& event)
	{
		const char* aID = event.GetCurrentElement()->GetAttribute<Rocket::Core::String>("action", "do nothing").CString();
		const char* eID = event.GetCurrentElement()->GetId().CString();
		dprintf(1, "Processing action \"%s\" in button element \"%s\"\n", aID, eID );
		std::string ID( aID );
		LrtClickFunc listen = listeners[ ID ];
		if( listen == NULL )
		{
			dprintf( 0, "Invalid listener\n" );
		}
		else
		{
			listen( listener_data[ ID ] );
		}
	}
};

class LrtLinkListener : public Rocket::Core::EventListener
{
public:
	
	void ProcessEvent(Rocket::Core::Event& event)
	{
		const char* aID = event.GetCurrentElement()->GetAttribute<Rocket::Core::String>("target", "invalid").CString();
		const char* eID = event.GetCurrentElement()->GetId().CString();
		dprintf(1, "Processing link to \"%s\" in element \"%s\"\n", aID, eID );
		std::string ID( aID );
		Rocket::Core::ElementDocument* target = documents[ ID ];
		if( target == NULL )
		{
			dprintf( 0, "Invalid target\n" );
		}
		else
		{
			active_document->Hide();
			active_document = target;
			active_document->Show();
		}
	}
};

static LrtButtonListener* lrtButtonListener;
static LrtLinkListener* lrtLinkListener;

static void loadDocument(const char* name, const char* ID)
{
	dprintf( 3, "Loading document \"%s\" with ID \"%s\"\n", name, ID );
	Rocket::Core::ElementDocument* document = rContext->LoadDocument(name);
	document->SetId(ID);
	if (document != NULL)
	{
		document->GetElementById("title")->SetInnerRML(document->GetTitle());
		document->Hide();
		document->RemoveReference();
	}
	
	Rocket::Core::ElementList links;
	Rocket::Core::ElementList buttons;
	
	document->GetElementsByTagName(links, "link");
	document->GetElementsByTagName(buttons, "button");
	
	for( auto &link : links )
	{
		dprintf( 5, "Listening to a link with ID \"%s\"\n", link->GetId().CString() );
		link->AddEventListener("click", lrtLinkListener, false);
	}
	for( auto &button : buttons )
	{
		dprintf( 5, "Listening to a button with ID \"%s\"\n", button->GetId().CString() );
		button->AddEventListener("click", lrtButtonListener, false);
	}
	
	documents[ std::string( ID ) ] = document;
	
	//document->GetElementById("main_exit")->AddEventListener("click", lrtButtonListener, false);
}

void initRocketMenu( void )
{
// Generic initialization
	if (!Shell::Initialise("./"))
	{
		Shell::Shutdown();
	}
	
	// Librocket requires this, i have no idea what this actually does.
	// I presume i'll need to rerun this if i need to change resolution.
	glClearColor(0, 0, 0, 1);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, 1024, 768, 0, -1, 1);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Rocket initialisation.
	Rocket::Core::SetRenderInterface(&opengl_renderer);
    opengl_renderer.SetViewport(1024, 768);

	Rocket::Core::SetSystemInterface(&system_interface);

	Rocket::Core::Initialise();

	// Create the main Rocket context and set it on the shell's input layer.
	rContext = Rocket::Core::CreateContext("soos", Rocket::Core::Vector2i(1024, 768));
	if (rContext == NULL)
	{
		Rocket::Core::Shutdown();
		Shell::Shutdown();
	}

	Rocket::Debugger::Initialise(rContext);
	Input::SetContext(rContext);

	Shell::LoadFonts("assets/");
	
	// Create the common listener
	lrtButtonListener = new LrtButtonListener();
	lrtLinkListener = new LrtLinkListener();

	// Load and show the tutorial document.
	
	loadDocument( "assets/Main Menu.rml", "main" );
	loadDocument( "assets/Map Editor.rml", "maped" );
	active_document = documents[ std::string( "main" ) ];
	active_document->Show();
}