//
//  main.c
//  eSudoku
//
//  Created by Andrew Williams on 15/04/2007.
//  Copyright Rectang.com 2007. All rights reserved.
//

#include "esudoku.h"
#include <Carbon/Carbon.h>

static OSStatus        AppEventHandler( EventHandlerCallRef inCaller, EventRef inEvent, void* inRefcon );
static OSStatus        HandleNew( );
static OSStatus        WindowEventHandler( EventHandlerCallRef inCaller, EventRef inEvent, void* inRefcon );
static OSStatus        HandleGenerate( WindowRef window );
static OSStatus        HandleSolve( WindowRef window );

static IBNibRef        sNibRef;

//--------------------------------------------------------------------------------------------
int main(int argc, char* argv[])
{
    OSStatus                    err;
    static const EventTypeSpec    kAppEvents[] =
    {
        { kEventClassCommand, kEventCommandProcess }
    };

    // Create a Nib reference, passing the name of the nib file (without the .nib extension).
    // CreateNibReference only searches into the application bundle.
    err = CreateNibReference( CFSTR("main"), &sNibRef );
    require_noerr( err, CantGetNibRef );
    
    // Once the nib reference is created, set the menu bar. "MainMenu" is the name of the menu bar
    // object. This name is set in InterfaceBuilder when the nib is created.
    err = SetMenuBarFromNib( sNibRef, CFSTR("MenuBar") );
    require_noerr( err, CantSetMenuBar );
    
    // Install our handler for common commands on the application target
    InstallApplicationEventHandler( NewEventHandlerUPP( AppEventHandler ),
                                    GetEventTypeCount( kAppEvents ), kAppEvents,
                                    0, NULL );
    
    // Create a new window. A full-fledged application would do this from an AppleEvent handler
    // for kAEOpenApplication.
    HandleNew();
    
    // Run the event loop
    RunApplicationEventLoop();

CantSetMenuBar:
CantGetNibRef:
    return err;
}

//--------------------------------------------------------------------------------------------
static OSStatus
AppEventHandler( EventHandlerCallRef inCaller, EventRef inEvent, void* inRefcon )
{
    OSStatus    result = eventNotHandledErr;
    
    switch ( GetEventClass( inEvent ) )
    {
        case kEventClassCommand:
        {
            HICommandExtended cmd;
            verify_noerr( GetEventParameter( inEvent, kEventParamDirectObject, typeHICommand, NULL, sizeof( cmd ), NULL, &cmd ) );
            
            switch ( GetEventKind( inEvent ) )
            {
                case kEventCommandProcess:
                    switch ( cmd.commandID )
                    {
                        case kHICommandNew:
                            result = HandleNew();
                            break;

                        // Add your own command-handling cases here
                        
                        default:
                            break;
                    }
                    break;
            }
            break;
        }
            
        default:
            break;
    }
    
    return result;
}

//--------------------------------------------------------------------------------------------
DEFINE_ONE_SHOT_HANDLER_GETTER( WindowEventHandler )

//--------------------------------------------------------------------------------------------
static OSStatus
HandleNew()
{
    OSStatus                    err;
    WindowRef                    window;
    static const EventTypeSpec    kWindowEvents[] =
    {
        { kEventClassCommand, kEventCommandProcess }
    };
    
    // Create a window. "MainWindow" is the name of the window object. This name is set in 
    // InterfaceBuilder when the nib is created.
    err = CreateWindowFromNib( sNibRef, CFSTR("MainWindow"), &window );
    require_noerr( err, CantCreateWindow );

    // Install a command handler on the window. We don't use this handler yet, but nearly all
    // Carbon apps will need to handle commands, so this saves everyone a little typing.
    InstallWindowEventHandler( window, GetWindowEventHandlerUPP(),
                               GetEventTypeCount( kWindowEvents ), kWindowEvents,
                               window, NULL );
    
    // Position new windows in a staggered arrangement on the main screen
    RepositionWindow( window, NULL, kWindowCascadeOnMainScreen );
    
    // The window was created hidden, so show it
    ShowWindow( window );
    
CantCreateWindow:
    return err;
}

//--------------------------------------------------------------------------------------------
static OSStatus
HandleGenerate( WindowRef window )
{
	ControlHandle grid[9][9];
	int x, y;
	char *value;
	
	value = malloc(2 * sizeof(char));
	generate(SCORE_MEDIUM);
	print_grid();
	for (y = 0; y < 9; y++)
	{
		for (x = 0; x < 9; x++)
		{
			int id = (y * 9) + x;
			ControlID gridControlID = {'grid', id};
			
			GetControlByID (window, &gridControlID, &grid[x][y]);

			if (main_grid[x][y] != -1)
			{
				snprintf(value, 2, "%d", main_grid[x][y]);
				SetControlData (grid[x][y], 0, kControlStaticTextTextTag, 2, value);
			}
			else
			{
				SetControlData (grid[x][y], 0, kControlStaticTextTextTag, 1, "");
			}
		}
	}
	free (value);

	return (OSStatus) 0;
}

//--------------------------------------------------------------------------------------------
static OSStatus
HandleSolve( WindowRef window )
{
	ControlHandle grid[9][9];
	int x, y, score;
	char *value;
	
	score = 0;
	value = malloc(2 * sizeof(char));

	for (y = 0; y < 9; y++) {
		for (x = 0; x < 9; x++) {
			int id = (y * 9) + x;
			ControlID gridControlID = {'grid', id};

			*value = 0;
			*(value+1) = 0;			
			GetControlByID (window, &gridControlID, &grid[x][y]);
			GetControlData (grid[x][y], 0, kControlStaticTextTextTag, 2, value, NULL);
			if (!value || !*value)
				main_grid[x][y] = -1;
			else
				main_grid[x][y] = atoi(value);
		}
	}

	solve(&score);
	print_grid();
	for (y = 0; y < 9; y++)
	{
		for (x = 0; x < 9; x++)
		{
			int id = (y * 9) + x;
			ControlID gridControlID = {'grid', id};
			
			GetControlByID (window, &gridControlID, &grid[x][y]);

			if (main_grid[x][y] >= 1)
			{
				snprintf(value, 2, "%d", main_grid[x][y]);
				SetControlData (grid[x][y], 0, kControlStaticTextTextTag, 2, value);
			}
			else
			{
				SetControlData (grid[x][y], 0, kControlStaticTextTextTag, 1, "");
			}
		}
	}
	free(value);

	return (OSStatus) 0;
}

//--------------------------------------------------------------------------------------------
static OSStatus
WindowEventHandler( EventHandlerCallRef inCaller, EventRef inEvent, void* inRefcon )
{
    OSStatus    err = eventNotHandledErr;
	WindowRef   window = (WindowRef) inRefcon;
    
    switch ( GetEventClass( inEvent ) )
    {
        case kEventClassCommand:
        {
            HICommandExtended cmd;
            verify_noerr( GetEventParameter( inEvent, kEventParamDirectObject, typeHICommand, NULL, sizeof( cmd ), NULL, &cmd ) );
            
            switch ( GetEventKind( inEvent ) )
            {
                case kEventCommandProcess:
                    switch ( cmd.commandID )
                    {
						case 'gen ':
							err = HandleGenerate( window );
							break;

						case 'solv':
							err = HandleSolve( window );
							break;

                        // Add your own command-handling cases here
                        
                        default:
                            break;
                    }
                    break;
            }
            break;
        }
            
        default:
            break;
    }
    
    return err;
}
