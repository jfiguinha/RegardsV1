//////////////////////////////////////////////////////////////////////////////////
// Initialise les tableaux.
//////////////////////////////////////////////////////////////////////////////////

var m_cellSelected;
var months = new Array("Janvier", "Février", "Mars",
        "Avril", "Mai", "Juin", "Juillet", "Août", "Septembre",
        "Octobre", "Novembre", "Décembre");
var daysInMonth = new Array(31, 28, 31, 30, 31, 30, 31, 31,
   30, 31, 30, 31);
var days = new Array("Dim", "Lun", "Mar",
     "Mer", "Jeu", "Ven", "Sam");

var iMouseX;
var iMouseY;

//////////////////////////////////////////////////////////////////////////////////
//Fonctions de sélection d'une date
//////////////////////////////////////////////////////////////////////////////////

function SelectedDate(iType)
{
	document.all['MSCAL'].style.visibility = 'visible';
	document.all['MSCAL'].style.top = iMouseY;
	document.all['MSCAL'].style.left = iMouseX;
	iTypeLocal = iType;
}

function checkValidDate(dateStr) 
{
    // dateStr must be of format month day year with either slashes
    // or dashes separating the parts. Some minor changes would have
    // to be made to use day month year or another format.
    // This function returns True if the date is valid.
    var slash1 = dateStr.indexOf("/");

    if (slash1 == -1) 
	{ 
		slash1 = dateStr.indexOf("-"); 
	}
    
	// if no slashes or dashes, invalid date
    if (slash1 == -1) 
	{ 
			return false; 
	}
    
	var dateDay = dateStr.substring(0, slash1);
    var dateMonthAndYear = dateStr.substring(slash1+1, dateStr.length);

    var slash2 = dateMonthAndYear.indexOf("/");
    if (slash2 == -1) 
	{ 
		slash2 = dateMonthAndYear.indexOf("-"); 
	}
    // if not a second slash or dash, invalid date
    if (slash2 == -1) 
	{ 
		return false; 
	}

    var dateMonth = dateMonthAndYear.substring(0, slash2);
    var dateYear = dateMonthAndYear.substring(slash2+1, dateMonthAndYear.length);

    if ( (dateMonth == "") || (dateDay == "") || (dateYear == "") ) 
	{ 
		return false; 
	}
    
	// if any non-digits in the month, invalid date
    for (var x=0; x < dateMonth.length; x++) 
	{
        var digit = dateMonth.substring(x, x+1);
        if ((digit < "0") || (digit > "9")) 
		{ 
			return false; 
		}
    }

    // convert the text month to a number
    var numMonth = 0;
    for (var x=0; x < dateMonth.length; x++) 
	{
        digit = dateMonth.substring(x, x+1);
        numMonth *= 10;
        numMonth += parseInt(digit);
    }

    if ((numMonth <= 0) || (numMonth > 12)) 
	{ 
		return false; 
	}
    
	// if any non-digits in the day, invalid date
    for (var x=0; x < dateDay.length; x++) 
	{
        digit = dateDay.substring(x, x+1);
        if ((digit < "0") || (digit > "9")) 
		{ 
			return false; 
		}
    }

    // convert the text day to a number
    var numDay = 0;
    for (var x=0; x < dateDay.length; x++) 
	{
        digit = dateDay.substring(x, x+1);
        numDay *= 10;
        numDay += parseInt(digit);
    }
    if ((numDay <= 0) || (numDay > 31)) 
	{ 
		return false; 
	}

    // February can't be greater than 29 (leap year calculation comes later)
    if ((numMonth == 2) && (numDay > 29)) 
	{ 
		return false; 
	}

    // check for months with only 30 days
    if ((numMonth == 4) || (numMonth == 6) || (numMonth == 9) || (numMonth == 11)) 
	{ 
        if (numDay > 30) 
		{ 
			return false; 
		} 
    }

    // if any non-digits in the year, invalid date
    for (var x=0; x < dateYear.length; x++) 
	{
        digit = dateYear.substring(x, x+1);
        if ((digit < "0") || (digit > "9")) 
		{ 
			return false; 
		}
    }
    // convert the text year to a number
    var numYear = 0;
    for (var x=0; x < dateYear.length; x++) 
	{
        digit = dateYear.substring(x, x+1);
        numYear *= 10;
        numYear += parseInt(digit);
    }

    // Year must be a 2-digit year or a 4-digit year
    if ( (dateYear.length != 2) && (dateYear.length != 4) ) 
	{ 
		return false; 
	}

    // if 2-digit year, use 50 as a pivot date
    if ( (numYear < 50) && (dateYear.length == 2) ) 
	{ 
		numYear += 2000; 
	}
    if ( (numYear < 100) && (dateYear.length == 2) ) 
	{ 
		numYear += 1900; 
	}

    if ((numYear <= 0) || (numYear > 9999)) 
	{ 
		return false; 
	}
    // check for leap year if the month and day is Feb 29
    if ((numMonth == 2) && (numDay == 29)) 
	{
        var div4 = numYear % 4;
        var div100 = numYear % 100;
        var div400 = numYear % 400;
        // if not divisible by 4, then not a leap year so Feb 29 is invalid
        if (div4 != 0) 
		{ 
			return false; 
		}
        // at this point, year is divisible by 4. So if year is divisible by
        // 100 and not 400, then it's not a leap year so Feb 29 is invalid
        if ((div100 == 0) && (div400 != 0)) 
		{ 
			return false; 
		}
    }
    // date is valid
    return true;
}



function DateSelection(Value, ValueStockage)
{
	if(iTypeLocal == 1)
	{
		formulaire.DateDe.value = Value;
		iValueDateDe = ValueStockage;
	}
	else
	{
		formulaire.DateA.value = Value;
		iValueDateA = ValueStockage;
	}

	document.all['MSCAL'].style.visibility = 'hidden';
}

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

function getDays(month, year) 
{
   // Teste si l'année est bissextile quand Février est sélectionné.
   if (1 == month)
      return ((0 == year % 4) && (0 != (year % 100))) ||
         (0 == year % 400) ? 29 : 28;
   else
      return daysInMonth[month];
}

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

function getToday() 
{
   // Génère la date du jour.
   this.now = new Date();
   this.year = this.now.getYear(); // Par rapport à 1900           
   this.month = this.now.getMonth();
   this.day = this.now.getDate();
}

// Commence avec le calendrier du mois en cours.
today = new getToday();

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

function newCalendar() 
{
   today = new getToday();
   var parseYear = parseFloat(document.all.annee[document.all.annee.selectedIndex].text);
   var newCal = new Date(parseYear,document.all.month.selectedIndex, 1);
   var day = -1;
   var startDay = newCal.getDay();
   var daily = 0;
   if ((today.year == newCal.getYear()) &&
         (today.month == newCal.getMonth()))
      day = today.day;
           
   // Mémorise l'élément tBody nommé dayList du tableau.
   var tableCal = document.all.calendar.tBodies.dayList;
   var intDaysInMonth = getDays(newCal.getMonth(), newCal.getYear());
   for (var intWeek = 0; intWeek < tableCal.rows.length; intWeek++)
   {
      for (var intDay = 0;intDay < tableCal.rows[intWeek].cells.length;intDay++) 
      {
         var cell = tableCal.rows[intWeek].cells[intDay];

         // Commence à compter les jours.
         if ((intDay == startDay) && (0 == daily))
            daily = 1;

         // Affiche la date du jour en gras.
         cell.className = (day == daily) ? "today" : "jour";
              
         if(cell.className == "today")
             cellSelected = cell;

         // Affiche le quantième dans la cellule.
         if ((daily > 0) && (daily <= intDaysInMonth))
            cell.innerText = daily++;
         else
            cell.innerText = "";
      }
    }
}

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

function jourSuivant(m_stgDate)
{
	var iModif = 0;
	var iPos = m_stgDate.indexOf("-");
	var iJour = m_stgDate.substring(0,iPos);
	iPos++;
	var iIndex = m_stgDate.indexOf("-",iPos);
	var iMois = m_stgDate.substring(iPos,iIndex);
	iIndex++;
	var iAnnee = m_stgDate.substring(iIndex,m_stgDate.length);

	iJour++;
	
	var iMaxDay = getDays(iMois-1, iAnnee);
	
	if(iJour > iMaxDay)
	{
		iModif = 1;
		iJour = 1;
		iMois++;
		if(iMois > 12)
		{
			iMois = 1;
			iAnnee++;
		}
	}
	var m_stgDate = "";

	if(iModif)
		m_stgDate = lead0(iJour) + "-" + lead0(iMois) + "-" + iAnnee;
	else
		m_stgDate = lead0(iJour) + "-" + iMois + "-" + iAnnee;
	
	return m_stgDate;
}

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

function jourPrecedent(m_stgDate)
{
	var iModif = 0;
	var iPos = m_stgDate.indexOf("-");
	var iJour = m_stgDate.substring(0,iPos);
	iPos++;
	var iIndex = m_stgDate.indexOf("-",iPos);
	var iMois = m_stgDate.substring(iPos,iIndex);
	iIndex++;
	var iAnnee = m_stgDate.substring(iIndex,m_stgDate.length);

	iJour--;
	
	if(iJour < 1)
	{
		iModif = 1;
		iMois--;
		
		if(iMois < 1)
		{
			iMois = 12;
			iAnnee--;
		}
		
		iJour = getDays(iMois-1, iAnnee);
	}
	var m_stgDate = "";

	if(iModif)
		m_stgDate = lead0(iJour) + "-" + lead0(iMois) + "-" + iAnnee;
	else
		m_stgDate = lead0(iJour) + "-" + iMois + "-" + iAnnee;
	
	return m_stgDate;
}

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

function ComparerDate(dtDate1, dtDate2)
{
	var iCompare1 = parseFloat(dtDate1.substr(6,4));
	var iCompare2 = parseFloat(dtDate2.substr(6,4));
	
	//Compare Année
	if(iCompare1 > iCompare2)
		return 1;
	
	if(iCompare1 < iCompare2)
		return -1;
		
	iCompare1 = parseFloat(dtDate1.substr(3,2));
	iCompare2 = parseFloat(dtDate2.substr(3,2));
	
	//Compare Mois
	if(iCompare1 > iCompare2)
		return 1;
	
	if(iCompare1 < iCompare2)
		return -1;	
		
	iCompare1 = parseFloat(dtDate1.substr(0,2));
	iCompare2 = parseFloat(dtDate2.substr(0,2));
	
	//Compare Jour		
	if(iCompare1 > iCompare2)
		return 1;
	
	if(iCompare1 < iCompare2)
		return -1;	
			
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

function writeCalendar()
{
	document.write("<DIV ID=\"MSCAL\" style=\"position:absolute;top:40px;visibility:hidden;\">");
	document.write("<table border=\"0\" cellpadding=\"0\" cellspacing=\"0\" width=\"233\" height=\"32\" bordercolor=\"#111111\" background=\"../GIF/barre_recherche.gif\" style=\"border-collapse: collapse\">");
	document.write("<tr>");
	document.write("<td width=\"46\" height=\"29\" align=\"center\"><img border=\"0\" src=\"../GIF/icone_barre_calendrier.gif\"></td>");
	document.write("<td width=\"143\" height=\"29\" valign=\"bottom\"><b>");
	document.write("<font size=\"2\" color=\"#FFFFFF\"><font face=\"Verdana\">Calendrier</font><br>");
	document.write("</font></b><img border=\"0\" src=\"../GIF/transp.gif\" width=\"20\" height=\"5\"></td>");
	document.write("<td width=\"46\" height=\"29\" valign=\"bottom\">");
	document.write("<p align=\"center\">");
	document.write("<a href=\"javascript:hideCalendar()\"><img border=\"0\" src=\"../GIF/icone_fermer.gif\" width=\"14\" height=\"13\"></a><br>");
	document.write("<img border=\"0\" src=\"../GIF/transp.gif\" width=\"20\" height=\"8\"><img border=\"0\" src=\"../GIF/transp.gif\" width=\"20\" height=\"3\"></td>");
	document.write("</tr>");
	document.write("</table>");
      	document.write("<TABLE ID=\"calendar\" border=\"1\"  width=\"233\" bgcolor=\"black\" bordercolorlight=\"#006699\" bordercolordark=\"#000000\" cellpadding=\"4\" bordercolor=\"#111111\" cellspacing=\"0\">");
        document.write("<THEAD>");
        document.write("<TR>");
        document.write("<TD COLSPAN=7 ALIGN=CENTER bgcolor=\"#EFF3FF\">");
        document.write("<SELECT ID=\"month\" ONCHANGE=\"newCalendar()\">");
        document.write("<SCRIPT LANGUAGE=\"JavaScript\">");
        document.write("writeMonth();");
        document.write("</SCRIPT>");
        document.write("</SELECT>");
        document.write("<SELECT ID=\"annee\" ONCHANGE=\"newCalendar()\">");
        document.write("<SCRIPT LANGUAGE=\"JavaScript\">");
        document.write("writeYear();");
        document.write("</SCRIPT>");
        document.write("</SELECT>");
        document.write("</TD>");
        document.write("</TR>");
        document.write("<TR CLASS=\"days\" bgcolor=\"#C0C0C0\">");
        document.write("<SCRIPT LANGUAGE=\"JavaScript\">");
        document.write("writeDay();");
        document.write("</SCRIPT>");
        document.write("</TR>");
        document.write("</THEAD>");
        document.write("<font face=\"verdana\" size=8>");
        document.write("<TBODY ID=\"dayList\"ALIGN=CENTER STYLE=\"cursor:hand\" ONCLICK=\"getDate()\" bgcolor=\"#E6E6E6\">");
        document.write("<SCRIPT LANGUAGE=\"JavaScript\">");
        document.write("writeWeeks();");
        document.write("</SCRIPT>");
        document.write("</TBODY>");
        document.write("</font>");
      	document.write("</TABLE>");
	document.write("</DIV>");
}

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

function writeWeeks()
{
      for (var intWeeks = 0; intWeeks < 6; intWeeks++) 
       {
          document.write("<TR>");
          for (var intDays = 0; intDays < days.length;intDays++)
             document.write("<TD></TD>");
             
          document.write("</TR>");
       }	
}

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

function writeDay()
{
  // Affiche les jours.
  for (var intLoop = 0; intLoop < days.length;
        intLoop++)
     document.write("<TD>" + days[intLoop] + "</TD>");
}

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

function writeYear()
{
	// Affiche les années dans le document.
	// Sélectionne l'année en cours.
	for (var intLoop = 2003; intLoop < 2020;
	      intLoop++)
	   document.write("<OPTION " +
	      (today.year == intLoop ?
	         "Selected" : "") + ">" +
	      intLoop);	
}

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

function writeMonth()
{
	// Affiche les mois dans le document.
	// Sélectionne le mois en cours.
	for (var intLoop = 0; intLoop < months.length; intLoop++)
	{
		var iMonth = intLoop + 1;
	   document.write("<OPTION value=" + iMonth + " " +
	      (today.month == intLoop ?
	         "Selected" : "") + ">" +
	      months[intLoop]);
	}	
}

//////////////////////////////////////////////////////////////////////////////////
//Fonction remplissant une liste de mois avec le mois courant comme valeur par
//défaut
//////////////////////////////////////////////////////////////////////////////////

function ecrireListeMois()
{
        var currDate = new Date();
        var iMois = currDate.getMonth() + 1;
        for(var i = 1;i < 13;i++)
        {
        	if(i == iMois)
        	{
        		document.write("<option value=\"" + i + "\" selected>" + months[i-1] + "</option>");
        	}
        	else
        	{
        		document.write("<option value=\"" + i + "\">" + months[i-1] + "</option>");
        	}
        }
}

//////////////////////////////////////////////////////////////////////////////////
//Fonction remplissant une liste d'année avec l'année courante comme valeur par
//défaut
//////////////////////////////////////////////////////////////////////////////////

function ecrireListeAnnee()
{
	var currDate = new Date();
        for(var i = 2003;i < 2020;i++)
        {
        	if(i == currDate.getYear())
        	{
        		document.write("<option value=\"" + i + "\" selected>" + i + "</option>");
        	}
        	else
        	{
        		document.write("<option value=\"" + i + "\">" +i + "</option>");
        	}
        }	
}	

//////////////////////////////////////////////////////////////////////////////////
//Fonction de récupération des valeurs d'une sélection
//////////////////////////////////////////////////////////////////////////////////
function changeMois()
{
	document.all['idDateMois'].checked = true;
}

function changeAnnee()
{
	document.all['idDateAnnee'].checked = true;
}	

function hideCalendar()
{
	document.all['MSCAL'].style.visibility = 'hidden';	
}

     function getDate() {
        // Ce code est exécuté quand l'utilisateur clique
        // sur un jour dans le calendrier.
        if ("TD" == event.srcElement.tagName)
           // Teste si le jour est valide.
           if ("" != event.srcElement.innerText)
           {
              var jour = event.srcElement.innerText;
              var parseYear = parseInt(document.all.annee[document.all.annee.selectedIndex].text);
			  var parseMonth = parseInt(document.all.month[document.all.month.selectedIndex].value);
			  var m_stgDate = jour + "/" + parseMonth + "/" + parseYear;
			  var m_stgDate2 = "0";
			  m_stgDate2 = parseYear;
			  if(parseMonth < 10)
				m_stgDate2 += "0" + parseMonth;
			  else
				m_stgDate2 += parseMonth;
			  if(jour < 10)
				m_stgDate2 += "0" + jour;
			  else
				m_stgDate2 += jour;
			  
			  cellSelected.className = "jour";
			  event.srcElement.className = "today";
			  cellSelected = event.srcElement;

			  DateSelection(m_stgDate,m_stgDate2);
           }
     }