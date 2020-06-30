///////////////////////////////////////////////////////////////////
//Texte à dire
///////////////////////////////////////////////////////////////////

var m_ctlAgentActiveX;
var merlin;
var iAide = 0;
var iLastId = 0;

var m_stgCadreFormulaire = "Este cadro contiene un formulario que permite definir los criterios de su investigación.";
var m_stgListeCritere = "Esta lista que desenrolla contiene los criterios que definen sus imágenes. Estos criterios pueden crearse y modificarse en el pequeño Álbum/Criterio.";
var m_stgEditLabel = "Esta caja permite definir los caracteres para buscar imagenes.";
var m_stgBoutonRecherche = "Apoyar en este botón para lanzar la investigación.";
var m_stgBoutonEtCheck = "Cochero la casilla Y fuerza el motor de investigación a buscar las imágenes que contienen todos los criterios definidos.";
var m_stgBoutonOuCheck = "Cochero la casilla O fuerza el motor de investigación a buscar las imágenes que contienen al menos uno de los criterios definidos.";

var m_stgCadreRechercheAvance = "Esta cojea de diálogo permite definir criterios de investigación más precisos sobre la imagen.";
var m_stgBoutonAffichage = "Al apoyar en esta imagen pueden hacer appara o dispara esta cojea de diálogo.";
var m_stgLibelleDateDe = "Esta caja permite de definir la fecha al menos.";
var m_stgLibelleDateA = "Esta caja permite de definir la fecha al mas tarde.";
var m_stgTypeImage = "Esta lista que desenrolla permite elegir el tipo de imagen que busque.";
var m_stgCalendrier = "Esta imagen permite evidenciar un calendario.";
var m_stgTaille = "Esta caja permite de especificar en kilo octeto el tamaño de la imagen.";
var m_stgListeTaille = "Esta lista que desenrolla sirve para indicar el tamaño mínimo o máximo de la imagen.";
var m_stgHauteur = "Esta caja permite de especificar el tamaño en pixel de la altura de la imagen.";
var m_stgLargeur = "Esta caja permite de especificar el tamaño en pixel de la anchura de la imagen.";

var m_stgAide = "Al presionar este vínculo harán la ayuda de Regards.";
var m_stgAssistant = "Este vínculo me evidenciará. Pulsar arriba para hacerme desaparecer.";
var m_stgPresentation = "Buenos días, soy Merlin su Ayudante por la investigación. Coloqúe me al frente de un elemento para obtener una explicación.";

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
	      		if(confirm("Veuillez télécharger le fichier genie.exe sur le site http://agent.microsoft.com"))
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