///////////////////////////////////////////////////////////////////
//Texte à dire
///////////////////////////////////////////////////////////////////

var m_ctlAgentActiveX;
var merlin;
var iAide = 0;
var iLastId = 0;

var m_stgCadreFormulaire = "This div contains a form to define the criteria for your search";
var m_stgListeCritere = "This drop-down list contains the criteria to define your images. These criteria can be create and modified in the menu Album/Critere.";
var m_stgEditLabel = "This dialog box define the characters which must be required in the picture.";
var m_stgBoutonRecherche = "Press this button to start the search.";
var m_stgBoutonEtCheck = "Check the box And forces the search engine to seek the pictures containing this criteria.";
var m_stgBoutonOuCheck = "Check the box Or forces the search engine to seek the pictures containing at less one of the definite criteria.";

var m_stgCadreRechercheAvance = "This dialog box allows to define search criteria more precise on picture.";
var m_stgBoutonAffichage = "While supporting on this picture you can show or unshow this dialog box.";
var m_stgLibelleDateDe = "This Edit box can define the sooner date.";
var m_stgLibelleDateA = "This Edit box can define the lastest date.";
var m_stgTypeImage = "This combo box use to select the format of picture.";
var m_stgCalendrier = "This picture can show a calandar.";
var m_stgTaille = "This edit box permit to search with the size criteria.";
var m_stgListeTaille = "This drop-down list is used to indicate the minimal or maximum size of a picture.";
var m_stgHauteur = "This dialog box allows use to indicate the size in pixel of the picture height.";
var m_stgLargeur = "This dialog box allows use to indicate the size in pixel of the picture width.";

var m_stgAide = "This permit to show the Regards Help.";
var m_stgAssistant = "This link will show me. Click on it to unshow me.";
var m_stgPresentation = "Hello, I am Merlin your search Wizard. Position me owe an element to obtain an explanation.";

function testActivexAgent()
{
 	if(iAide == 0)
 	{
		m_ctlAgentActiveX = new ActiveXObject("Agent.Control.2");
		if (m_ctlAgentActiveX)
		{
			 m_ctlAgentActiveX.Connected = true;
			 m_ctlAgentActiveX.Characters.Load("Merlin", "merlin.acs");
			 merlin = m_ctlAgentActiveX.Characters.Character("Merlin");
			 merlin.MoveTo(iMouseX,iMouseY);
			 merlin.LanguageID = 0x040C;
			 merlin.Show();
			 merlin.Speak(m_stgPresentation);
			 //merlin.Think("I beg to make no mistake yet!");
	      	}
	      	else
	      	{
	      		if(confirm("You must download genie.exe on the web site http://agent.microsoft.com"))
	      			window.open("http://agent.microsoft.com","_new");	
	      	}
	      iAide = 1;
	}
	else
	{
		merlin.hide();
		iAide = 0;
	}
	
}

function stopActivex()
{
	if(iAide) 
		merlin.Stop();
}

function affichageAide(m_stgLibelle, id)
{
	if(iAide && (iLastId != id))
	{
		merlin.Stop();
		merlin.MoveTo(iMouseX,iMouseY);
		merlin.Speak(m_stgLibelle);
		iLastId = id;
	}
}