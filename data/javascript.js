var config = {};
var skyStatus={"moon":false, "stars":false, "phase":0, "mode":0};

var MODE = { MANUAL:0, ACTIVE:1, RELAX:2};
var DAYS = ["Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"];

function init() {
	//loads config
	var xhttp = new XMLHttpRequest();
	xhttp.onreadystatechange = function() {
		if (this.readyState == 4 && this.status == 200) {
		  //TODO response 
		  config = JSON.parse(this.responseText);
		  confManager.setUI();
		}
	};
	xhttp.open("GET", "config", true);
	xhttp.send();
	
	statusManager.updateStatus();
}

// getElementById shorthand
function gh(id){
 return document.getElementById(id);
}
// transforms a second time into a hh:mm format 
function formatTime(ms, def_val) {
  if (Number.isNaN(ms)) {
    return (def_val)?def_val:null;
  } else {
    return [
        parseInt(ms / 60 / 60 ),
        parseInt(ms / 60 % 60 )
    ]
    .join(":")
    .replace(/\b(\d)\b/g, "0$1");
  }
} 

/**
 * manages the status of the sky
 */
var statusManager = {
	updateUI: function() {
		gh("moonlight").checked = skyStatus.moon;
		gh("starslight").checked = skyStatus.stars;
		gh("moonphase").value = skyStatus.phase;
		this.setPhaseGradiant(skyStatus.phase);
		
		if(skyStatus.phaseManual) {
			gh("phaseManual").checked = "checked";
			gh("moonphase").style.visibility= "visible";
		} else {
			gh("phaseAuto").checked = "checked";
			gh("moonphase").style.visibility= "hidden";
		}
	},
	toggledMoon: function() {
		skyStatus.moon = gh("moonlight").checked;
		this.changeStatus("moon", skyStatus.moon);
	},
	toggledStars: function() {
		skyStatus.stars = gh("starslight").checked;
		this.changeStatus("stars", skyStatus.stars);
	},
	toggledPhase: function() {
		skyStatus.phase = gh("moonphase").value;
		this.setPhaseGradiant(skyStatus.phase);
		this.changeStatus("phase", skyStatus.phase);
	},
	changeStatus: function(what,value){
		var that = this;
		var xhttp = new XMLHttpRequest();
		xhttp.onreadystatechange = function() {
			if (this.readyState == 4 && this.status == 200) {
			  skyStatus = JSON.parse(this.responseText);
			  that.updateUI();
			}
		};
		xhttp.open("PUT", "changeStatus?" + what + "=" + value, true);
		xhttp.setRequestHeader("Content-Type", "text/plain");
		xhttp.send();
	},
	updateStatus: function() {
		var that = this;
		var xhttpStatus = new XMLHttpRequest();
		xhttpStatus.onreadystatechange = function() {
			if (this.readyState == 4 && this.status == 200) {
			  skyStatus = JSON.parse(this.responseText);
			  that.updateUI();
			}
		};
		xhttpStatus.open("GET", "status", true);
		xhttpStatus.send();
	},
	phaseMode: function() {
		skyStatus.phaseManual = gh("phaseManual").checked;
		this.changeStatus("phaseManual", skyStatus.phaseManual);
		if (skyStatus.phaseManual) {
			gh("moonphase").style.visibility= "visible";
		} else {
			gh("moonphase").style.visibility= "hidden";
		}
	},
	setPhaseGradiant: function(per) {
		var dg = per / 180 * 100;
		gh('gradiant').style.background = "linear-gradient(to right, rgba(0,0,0,0.95) "+dg+"%,rgba(255, 255, 255, 0) 100%)";
	}
};								  
// configuration manager
var confManager = {
  maxSchedItem:7,
  uiOpened:false,
  editMode:null,
  editId:null,
  editWake:null,
  // toggles between open and close config UI
  toggled: function(val) {
    // closing ui config screen
    if (this.uiOpened == true && val == false) {
		//alert('do saving magic');
		this.getUI();
		this.save();
	}
	this.uiOpened = val;
  },
  changeMode: function(){
	this.getUImode();
	var xhttp = new XMLHttpRequest();
	xhttp.onreadystatechange = function() {
		if (this.readyState == 4 && this.status == 200) {
			//console.log(this.responseText);
		}
	};
	xhttp.open("PUT", "mode?mode=" + config.mode, true);
	xhttp.setRequestHeader("Content-Type", "text/plain");
	xhttp.send();
  },
  showPopUp: function(item, mode, wake, day) {
	this.editMode = mode;
	this.editId = item;
	this.editWake = wake;
	
	if (this.editMode==1) {
		c = config.active;
	} else {
		c = config.relax;			
	}
	if (this.editWake) {
		pTime = c[this.editId].wakeTime;
		pMoon = c[this.editId].wakeMoon;
		pStars = c[this.editId].wakeStars;
		gh('popupType').innerHTML = "&#127774;";
	} else {
		pTime = c[this.editId].sleepTime;
		pMoon = c[this.editId].sleepMoon;
		pStars = c[this.editId].sleepStars;
		gh('popupType').innerHTML = "&#128564;";
	}
		
	gh('pTime').valueAsNumber = pTime * 1000;
	gh('pMoonSleep').value = pMoon;
	gh('pStarsSleep').value = pStars;
	gh("timeEdit").style.display = "block";
	gh('popupDay').innerHTML = DAYS[day];
	
  },
  hidePopUp: function(set) {
	var mk, c, pTime, pMoon, pStars;
	
	if (set && this.editMode!= null && this.editWake != null) {
		pTime = gh('pTime').valueAsNumber / 1000;
		pMoon = gh('pMoonSleep').value;// | 30;
		pStars = gh('pStarsSleep').value;// | 45;
		
		if(this.editMode==1) {
			mk = "act_" + this.editId;
			c = config.active;
		} else {
			mk = "rel_" + this.editId;
			c = config.relax;			
		}
		
		if (this.editWake) {
			//Time
			c[this.editId].wakeTime = pTime;
			gh('wakeTime_'+mk).innerHTML = formatTime(pTime, "00:00");
			//Moon
			c[this.editId].wakeMoon = pMoon;
			gh('wakeMoon_'+mk).innerHTML = pMoon;
			//Stars
			c[this.editId].wakeStars = pStars;
			gh('wakeStars_'+mk).innerHTML = pStars;
		} else {
			//Time
			c[this.editId].sleepTime = pTime;
			gh('sleepTime_'+mk).innerHTML = formatTime(pTime, "00:00");
			//Moon
			c[this.editId].sleepMoon = pMoon;
			gh('sleepMoon_'+mk).innerHTML = pMoon;
			//Stars
			c[this.editId].sleepStars = pStars;
			gh('sleepStars_'+mk).innerHTML = pStars;
		}
	}
	gh("timeEdit").style.display = "none";
	this.editId = null;
	this.editMode = null;
	this.editWake = null;
  },
  getUImode: function() {
	  if(gh('modeManual').checked) {
		config.mode = MODE.MANUAL;
	} else if(gh('modeRelax').checked) {
		config.mode = MODE.RELAX;
	} else {
		config.mode = MODE.ACTIVE;
	}
  },
  getUI: function() {
    //config.moonWake = gh('moonwakeDuration').value;
	config.moonSleep = gh('moonsleepDuration').value;
	//config.starsWake = gh('starwakeDuration').value;
	config.starsSleep = gh('starsleepDuration').value;
	this.getUImode();

	var mk, k, c;
	for (var j = 0; j < 2 ; j++) {
		if(j==0){
		  mk = "act_";
		  c = config.active;				  
		} else {
			mk = "rel_";
			c = config.relax;			
		}
		for(var i = 0; i < this.maxSchedItem; i++) {
			k = mk + i;
			c[i].wakeOn = gh('wakeOn_'+k).checked;
			c[i].sleepOn = gh('sleepOn_'+k).checked; 
		}
	}
  },
  setUI: function() {
	//gh('moonwakeDuration').value = config.moonWake;
	gh('moonsleepDuration').value = config.moonSleep;
	//gh('starwakeDuration').value = config.starsWake;
	gh('starsleepDuration').value = config.starsSleep;
	switch(config.mode) {
		case 0: gh('modeManual').checked = true; break;
		case 1: gh('modeActive').checked = true; break;
		case 2: gh('modeRelax').checked = true; break;
	}
	
	var mk, k, c, el;
	for (var j = 0; j < 2 ; j++) {
		if(j==0){
		  mk = "act_";
		  c = config.active;
		  el = "activeSchedule";
		} else {
			mk = "rel_";
			c = config.relax;
			el = "relaxSchedule";
		}
		for(var i = 0; i < this.maxSchedItem; i++) {
			k = mk + i;
			this.addSchedItem("schedule-item", el, {"#KEY#":k, "#ID#":i,"#MODE#":j+1,"#DAY#":DAYS[i]});
									 
			gh('wakeOn_'+k).checked = c[i].wakeOn;
			gh('sleepOn_'+k).checked = c[i].sleepOn;
			gh('wakeTime_'+k).innerHTML = formatTime(c[i].wakeTime, "00:00");
			gh('sleepTime_'+k).innerHTML = formatTime(c[i].sleepTime, "00:00");
			gh('wakeMoon_'+k).innerHTML = c[i].wakeMoon;
			gh('wakeStars_'+k).innerHTML = c[i].wakeStars;
			gh('sleepMoon_'+k).innerHTML = c[i].sleepMoon;
			gh('sleepStars_'+k).innerHTML = c[i].sleepStars;
		}
	}
  },
  addSchedItem: function(template, targetParent, dict) {
        var t = document.querySelector('#' + template);
        var clone = t.cloneNode(true)
        for (key in dict) {
            clone.innerHTML = clone.innerHTML.replaceAll(key, dict[key])
        }
        var fragment = document.importNode(clone.content, true)
        var canvas = document.querySelector('#' + targetParent);
        canvas.appendChild(fragment);
  },
  save: function(){
	var xhttp = new XMLHttpRequest();
	xhttp.onreadystatechange = function() {
		if (this.readyState == 4 && this.status == 200) {
			//console.log(this.responseText);
		}
	};
	xhttp.open("POST", "saveconfig", true);
	xhttp.send("data="+JSON.stringify(config));
  }
};
