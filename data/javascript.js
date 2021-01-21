var config = {};

//var config={"moonWake":30,"moonSleep":30,"starsWake":45,"starsSleep":45,"active":[{"isOn":true,"sTime":"07:00","onWake":true,"mo":true,"tu":true,"we":true,"th":true,"fr":true,"sa":false,"su":false},{"isOn":true,"sTime":"21:00","onWake":false,"mo":true,"tu":true,"we":true,"th":true,"fr":true,"sa":false,"su":false},{"isOn":true,"sTime":"09:00","onWake":true,"mo":false,"tu":false,"we":false,"th":false,"fr":false,"sa":true,"su":true},{"isOn":true,"sTime":"23:00","onWake":false,"mo":false,"tu":false,"we":false,"th":false,"fr":false,"sa":true,"su":true},{"isOn":false,"sTime":null,"onWake":false,"mo":false,"tu":false,"we":false,"th":false,"fr":false,"sa":false,"su":false},{"isOn":false,"sTime":null,"onWake":false,"mo":false,"tu":false,"we":false,"th":false,"fr":false,"sa":false,"su":false},{"isOn":false,"sTime":null,"onWake":false,"mo":false,"tu":false,"we":false,"th":false,"fr":false,"sa":false,"su":false},{"isOn":false,"sTime":null,"onWake":false,"mo":false,"tu":false,"we":false,"th":false,"fr":false,"sa":false,"su":false},{"isOn":false,"sTime":null,"onWake":false,"mo":false,"tu":false,"we":false,"th":false,"fr":false,"sa":false,"su":false},{"isOn":false,"sTime":null,"onWake":false,"mo":false,"tu":false,"we":false,"th":false,"fr":false,"sa":false,"su":false},{"isOn":false,"sTime":null,"onWake":false,"mo":false,"tu":false,"we":false,"th":false,"fr":false,"sa":false,"su":false},{"isOn":false,"sTime":null,"onWake":false,"mo":false,"tu":false,"we":false,"th":false,"fr":false,"sa":false,"su":false},{"isOn":false,"sTime":null,"onWake":false,"mo":false,"tu":false,"we":false,"th":false,"fr":false,"sa":false,"su":false},{"isOn":false,"sTime":null,"onWake":false,"mo":false,"tu":false,"we":false,"th":false,"fr":false,"sa":false,"su":false}],"relax":[{"isOn":true,"sTime":"09:00","onWake":true,"mo":true,"tu":true,"we":true,"th":true,"fr":true,"sa":false,"su":false},{"isOn":true,"sTime":"22:00","onWake":false,"mo":true,"tu":true,"we":true,"th":true,"fr":true,"sa":false,"su":false},{"isOn":true,"sTime":"10:00","onWake":true,"mo":false,"tu":false,"we":false,"th":false,"fr":false,"sa":true,"su":true},{"isOn":true,"sTime":"23:00","onWake":false,"mo":false,"tu":false,"we":false,"th":false,"fr":false,"sa":true,"su":true},{"isOn":false,"sTime":null,"onWake":false,"mo":false,"tu":false,"we":false,"th":false,"fr":false,"sa":false,"su":false},{"isOn":false,"sTime":null,"onWake":false,"mo":false,"tu":false,"we":false,"th":false,"fr":false,"sa":false,"su":false},{"isOn":false,"sTime":null,"onWake":false,"mo":false,"tu":false,"we":false,"th":false,"fr":false,"sa":false,"su":false},{"isOn":false,"sTime":null,"onWake":false,"mo":false,"tu":false,"we":false,"th":false,"fr":false,"sa":false,"su":false},{"isOn":false,"sTime":null,"onWake":false,"mo":false,"tu":false,"we":false,"th":false,"fr":false,"sa":false,"su":false},{"isOn":false,"sTime":null,"onWake":false,"mo":false,"tu":false,"we":false,"th":false,"fr":false,"sa":false,"su":false},{"isOn":false,"sTime":null,"onWake":false,"mo":false,"tu":false,"we":false,"th":false,"fr":false,"sa":false,"su":false},{"isOn":false,"sTime":null,"onWake":false,"mo":false,"tu":false,"we":false,"th":false,"fr":false,"sa":false,"su":false},{"isOn":false,"sTime":null,"onWake":false,"mo":false,"tu":false,"we":false,"th":false,"fr":false,"sa":false,"su":false},{"isOn":false,"sTime":null,"onWake":false,"mo":false,"tu":false,"we":false,"th":false,"fr":false,"sa":false,"su":false}]};
var status={"starsOn":false, "moonOn":false, "moonMode":null, "moonphase":0};
function init(){
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
}
// getElementById shorthand
function gh(id){
 return document.getElementById(id);
}

function formatTime(ms, def_val) {
  if (Number.isNaN(ms)) {
    return (def_val)?def_val:null;
  } else {
    return [
        parseInt(ms / 60 / 60 / 1000),
        parseInt(ms / 60 / 1000 % 60 )
    ]
    .join(":")
    .replace(/\b(\d)\b/g, "0$1");
  }
} 
//TODELETE for test only
function test(){
console.log(gh('moonwakeDuration').value);
}
// configuration manager
var confManager = {
  maxSchedItem:14,
  uiOpened:false,
  updateUI: function() {
	alert('worked');
  },
  toggled: function(val) {
    // closing ui config screen
    if(this.uiOpened == true && val == false) {
		//alert('do saving magic');
		this.getUI();
		this.save();
	}
	this.uiOpened = val;
  },
  getUI: function() {
    config.moonWake = gh('moonwakeDuration').value;
	config.moonSleep = gh('moonsleepDuration').value;
	config.starsWake = gh('starwakeDuration').value;
	config.starsSleep = gh('starsleepDuration').value;
	
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
			c[i].isOn = gh('isOn_'+k).checked;
			c[i].sTime = gh('sTime_'+k).valueAsNumber;
			c[i].onWake = gh('onWake_'+k).checked;
			c[i].mo = gh('mo_'+k).checked;
			c[i].tu = gh('tu_'+k).checked;
			c[i].we = gh('we_'+k).checked;
			c[i].th = gh('th_'+k).checked;
			c[i].fr = gh('fr_'+k).checked;
			c[i].sa = gh('sa_'+k).checked;
			c[i].su = gh('su_'+k).checked;	
		}
	}
	
  },
  setUI: function() {
	gh('moonwakeDuration').value = config.moonWake;
	gh('moonsleepDuration').value = config.moonSleep;
	gh('starwakeDuration').value = config.starsWake;
	gh('starsleepDuration').value = config.starsSleep;
	
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
			this.addSchedItem("schedule-item", el, {"#KEY#":k});
			gh('isOn_'+k).checked = c[i].isOn;
			gh('sTime_'+k).value = formatTime(c[i].sTime, "00:00");
			if(c[i].onWake) {
			  gh('onWake_'+k).checked = true;
			} else {
			  gh('onSleep_'+k).checked = true;
			}
			gh('mo_'+k).checked = c[i].mo;
			gh('tu_'+k).checked = c[i].tu;
			gh('we_'+k).checked = c[i].we;
			gh('th_'+k).checked = c[i].th;
			gh('fr_'+k).checked = c[i].fr;
			gh('sa_'+k).checked = c[i].sa;
			gh('su_'+k).checked = c[i].su;
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
		  //TODO response
			console.log(this.responseText);		  
		  //document.getElementById("ADCValue").innerHTML = this.responseText;
		}
	};
	xhttp.open("POST", "saveconfig", true);
	xhttp.send("data="+JSON.stringify(config));
  }
};

function updateTextInput(val) {
  document.getElementById('textInput').innerHTML=val; 
}


