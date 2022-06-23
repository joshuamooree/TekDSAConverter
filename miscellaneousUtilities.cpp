#include "miscellaneousUtilities.h"
#include <stdarg.h>

//can't be class scope since OnUpdateProgress must be
//static to be a callback
wxProgressDialog* progress;
bool OnUpdateProgress( int percent )
{
    if( progress == NULL )
        progress = new wxProgressDialog( _("Converting picture"),
                                         _("Percent complete"),
                                         100,
                                         NULL,
                                         wxPD_AUTO_HIDE |
                                         wxPD_APP_MODAL |
                                         wxPD_CAN_ABORT );

    //did somebody click abort?
    bool returnValue = progress -> Update( percent );

    //delete the progress box if somebody clicked canceled
    //or if the conversion is over
    if( percent >= 100 || returnValue == false )
    {
        delete progress;
        progress = NULL;
    }

    //update the screen (keeps the window from getting obscured if
    //somebody minimizes it).  While this might not be the best way
    //to do this, it is the best way I can think of for this situation.
    wxTheApp -> GetTopWindow() ->Update();

    //return what we found for the abort thing
    return returnValue;
}


void AutoSavePicture( wxBitmap bitmap )
{
    //the number for the file.  The initial value is stored in the config
    int fileCount;
    //complete filename
    wxString currentFile;
    //the file name without the path, the number or the extension
    static wxString fileNameBase;
    //the rest of the variable are setup in the config section

    /*-----------------------------------------*/
    /* config stuff                            */
    /*-----------------------------------------*/
    //get the config object
    wxConfigBase* config = wxConfigBase::Get();

    bool autosave;
    config ->Read( _T("/Main/Autosave"), &autosave, true );

    //if we shouldn't autosave, no point in continuing
    if(!autosave)
        return;

    //save the existing path to be nice
    wxString previousPath = config ->GetPath();

    config ->SetPath( _T("/Autosave/") );

    //a cache for the name specifier.  Keeps us from having to do all
    //the replaces every time
    static wxString lastNameSpecifier;
    //get the name specifier from the config
    wxString nameSpecifier = config ->Read( _T("Name Specifier"), _T("screenshot %d "));
    //get the file type form the config
    wxString fileType = config ->Read( _T("File type"), _T(".png") );
    //get the path from the config
    wxString savePath = config ->Read( _T("Path"), _T("./") );
    //get the autosave count value from the config.  This keeps the program from
    //having to try possibly hundreds of files before it finds the right one.
    config ->Read( _T("Autosave Count"), &fileCount, 0 );

    /*-----------------------------------------*/
    /* end config stuff                        */
    /*-----------------------------------------*/

    //lets see if the name specifier changed
    if( nameSpecifier.Cmp(lastNameSpecifier) != 0 )
    {
        //oh, so it did change.  Well, update the config string and
        //lastNameSpecifier
        fileNameBase = SubstituteCharacters( nameSpecifier );
        lastNameSpecifier = nameSpecifier;
    }

    //search until we find a filename not taken yet.
    //But first, we need to see if somebody deleted the files.  If they did, we
    //need to search backwards until we find the files.  The loop for this test
    //should normally not run at all.

    //make the test filename
    currentFile.Printf( _T( "%s%s%d%s" ),
                        savePath.c_str(),
                        fileNameBase.c_str(),
                        fileCount - 1,
                        fileType.c_str() );

    //loop until we either find a file or fileCount reaches 0
    while( !wxFileName::FileExists( currentFile ) && fileCount > 0 )
    {
        //make the test filename
        currentFile.Printf( _T( "%s%s%d%s" ),
                            savePath.c_str(),
                            fileNameBase.c_str(),
                            fileCount,
                            fileType.c_str() );
        //decrement the filename
        fileCount--;
    }

    //now the forward search.  This loop should normally only run once due to the
    //caching
    do
    {
        //make the filename
        currentFile.Printf( _T( "%s%s%d%s" ),
                            savePath.c_str(),
                            fileNameBase.c_str(),
                            fileCount,
                            fileType.c_str() );
        //increment the filename
        fileCount++;
    //loop until the filename is not found
    } while( wxFileName::FileExists( currentFile ) );


    //try to save the file
    if( !bitmap.ConvertToImage().SaveFile( currentFile ) )
        //error check
        wxLogError( _("Could not autosave file: %s"), currentFile.c_str() );

    //cache fileCount
    config ->Write( _T("Autosave count"), fileCount );

    //be nice, restore the config path
    config ->SetPath( previousPath );

}   //end method AutoSavePicture

wxString SubstituteCharacters( wxString replacementString )
{
    wxString result( replacementString );
    //here I am replacing an escaped % character with a % and a dummy character
    result.Replace( _T("%%"), _T("%\1") );

    //process date - %d
    result.Replace( _T("%d"), wxDateTime::Now().FormatDate() );

    //process time - %t
    result.Replace( _T("%t"), wxDateTime::Now().FormatTime() );

    //process year - %y
    result.Replace( _T("%y"), wxString::Format( _T("%d"), wxDateTime::Now().GetYear() ) );

    //process month - %M
    result.Replace( _T("%M"), wxDateTime::GetMonthName( wxDateTime::Now().GetMonth() ) );

    //process hour - %h
    result.Replace( _T("%h"), wxString::Format( _T("%d"), wxDateTime::Now().GetHour() ) );

    //process minute - %m
    result.Replace( _T("%m"), wxString::Format( _T("%d"), wxDateTime::Now().GetMinute() ) );

    //process second - %s
    result.Replace( _T("%s"), wxString::Format( _T("%d"), wxDateTime::Now().GetSecond() ) );

    //final step for the escaped %.  Replace the dummy characters with the final one
    result.Replace( _T("%\1"), _("%") );

    //last steps.  Find invalid characters and replace them with _
    result.Replace( _T("\\"), _T("_") );
    result.Replace( _T("/"), _T("_") );
    result.Replace( _T("<"), _T("_") );
    result.Replace( _T(">"), _T("_") );
    result.Replace( _T(":"), _T("_") );
    result.Replace( _T("?"), _T("_") );
    result.Replace( _T("*"), _T("_") );
    result.Replace( _T("\""), _T("_") );
    result.Replace( _T("|"), _T("_") );

    return result;
}   //end method SubstituteCharacters


void LogAtLevel(int level, const wxChar *szFormat, ...)
{
    va_list argptr;
    va_start(argptr, szFormat);


    wxLog* oldTarget = wxLog::GetActiveTarget();

    long logLevel = wxConfigBase::Get() ->Read("/Log/Level", 0l );

    if(logLevel > 0)
    {
        static FILE* fp = fopen( "log.txt", "w" );

        wxLog::SetActiveTarget( new wxLogStderr(fp) );
    }

    if(level <= logLevel)
        wxVLogMessage ( szFormat, argptr );
    va_end(argptr);

    wxLog::SetActiveTarget(oldTarget);
}
