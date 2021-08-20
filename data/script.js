function logoutButton() {
  var xhr = new XMLHttpRequest();
  xhr.open("GET", "/logout", true);
  xhr.send();
  setTimeout(function(){ window.open("/logged-out","_self"); }, 1000);
}
function update() {
  var xhr = new XMLHttpRequest();
  xhr.open("GET", "/update", true);
  xhr.send();
  setTimeout(function(){ window.open("/update","_self"); }, 1000);
}
function setup() {
    var xhr = new XMLHttpRequest();
    xhr.open("GET", "/setup", true);
    xhr.send();
    setTimeout(function(){ window.open("/setup","_self"); }, 1000);
  }

  
  function reboot() {
    var xhr = new XMLHttpRequest();
    xhr.open("GET", "/reboot", true);
    xhr.send();
    setTimeout(function(){ window.open("/reboot","_self"); }, 1000);
  }

  function homepage() {
    var xhr = new XMLHttpRequest();
    xhr.open("GET", "/", true);
    xhr.send();
    setTimeout(function(){ window.open("/","_self"); }, 1000);
  }

function resetfactory() {
   var xhr = new XMLHttpRequest();
   xhr.open("GET", "/reset", true);
   xhr.send();
   setTimeout(function(){ window.open("/reset","_self"); }, 1000);
 }
