#define INCL_WINSHELLDATA
#include <os2.h>

#include <string.h>
#include <malloc.h>

#define  APP_NAME "SYS_DLLS"
#define  KEY_NAME "LoadOneTime"
#define  DLL_NAME "YOURDLL"

main()
{
  HAB   hAB;
  PSZ   pszProfile;
  PVOID pTemp;
  ULONG ulLength;

  hAB = WinInitialize(0);

  /*-------------------------------------------------------------------
    Query the current KEY_NAME's string length
   -------------------------------------------------------------------*/
  if (!(PrfQueryProfileSize (HINI_USERPROFILE,
                             APP_NAME,
                             KEY_NAME,
                             &ulLength)))
  {
    exit();
  }

  /*-------------------------------------------------------------------
    Calculate the new KEY_NAME's string length, allowing for a blank
    delimiter and NULL terminator, and allocate a buffer capable of
    holding the new string
   -------------------------------------------------------------------*/
  ulLength += sizeof(DLL_NAME) + 2;
  pszProfile = malloc(ulLength);

  /*-------------------------------------------------------------------
    Query the current KEY_NAME's string ... Exit the pgm if an
    error occurs
   -------------------------------------------------------------------*/
  if (!(PrfQueryProfileString (HINI_USERPROFILE,
                               APP_NAME,
                               KEY_NAME,
                               NULL,
                               pszProfile,
                               ulLength)))
  {
    free(pszProfile);
    exit();
  }

  /*-------------------------------------------------------------------
    Add the blank delimiter iff the current KEY_NAME string contains
    something
   -------------------------------------------------------------------*/
  if (strlen(pszProfile))
    strcat(pszProfile, " ");       /*Blank delimiter */

  /*-------------------------------------------------------------------
    Add your DLL name to the KEY_NAME string
   -------------------------------------------------------------------*/
  strcat(pszProfile, DLL_NAME);

  /*-------------------------------------------------------------------
    Write the new KEY_NAME string to your OS2.INI file
   -------------------------------------------------------------------*/
  PrfWriteProfileString (HINI_USERPROFILE,
                         APP_NAME,
                         KEY_NAME,
                         pszProfile);

  /*-------------------------------------------------------------------
    Clean up and exit stage right
   -------------------------------------------------------------------*/
  free(pszProfile);
  exit();

}
