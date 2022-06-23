#ifndef _MISCELLANEOUSUTILITIES_H_
#define _MISCELLANEOUSUTILITIES_H_

#include "DSA_11k.h"

//can't be class scope since OnUpdateProgress must be
//static to be a callback
extern wxProgressDialog* progress;

/**
 * \param percent how far the process has progressed as a percent out of 100
 * \return false if the user canceled, true otherwise
 * Does the progress bar.  Intended to be a callback.  Set percent to 100
 * to get rid of the progress bar.
 */
bool OnUpdateProgress( int percent );

/**
 * \param path the path to the config stuff this should use.  Defaults to /Autosave/
 * \param bitmap the bitmap to save.  Easy enough.
 * This function works with SubstituteCharacters
 * to implement the autosave functionality.  This can be used in more than one
 * place by specifying path differently for each place.  Otherwise, they
 * will interfere.
 */
void AutoSavePicture( wxBitmap bitmap );

/**
 * \param replacementString the string that should have its characters replaced
 * \return the string with all the appropriate characters replaced
 * The purpose of this function is to do some control character replacements
 * and to replace characters that the filesystem cannot deal with with a _
 * character.  The current control characters are:
 * %% - % character
 * %d - current date
 * %t - current time
 * %y - current year
 * %M - current month (name, not number)
 * %h - current hour
 * %m - current minute
 * %s - current second
 *
 * For Windowes, the invalid characters are \/<>:?*"|
 */
wxString SubstituteCharacters( wxString replacementString );

void LogAtLevel(int level, const wxChar *szFormat, ...);


#endif //_MISCELLANEOUSUTILITIES_H_
