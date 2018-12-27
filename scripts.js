var name;
function timer(arg){
  var time = arg;
  setInterval(function() {
	  document.getElementById("timer").innerHTML = time + "s";
	  time = time - 1;
	  if (time < 0) {
		time = 60;
	  }
  }, 1000);
}

function requestTurnResult() {
//window.onload = function(){
	//name = window.prompt("Please put your user name");
	var obj={
		"users": [
			{"name": "baton96",
			"answears": ["polska","poznan","paulina","pomaranczowy","papuga","pomidor"]},
			{"name": "emilia96",
			"answears": ["polska1","poznan2","paulina3","pomaranczowy4","papuga5","pomidor6"]}
		]
	}
	var myTable = document.getElementById("myTable");
	//myTable.removeChild(myTable.firstChild);
	for(i in obj.users){
		user = obj.users[i];

		var tr = document.createElement("tr");
		var td = document.createElement("td");
		tr.appendChild(td);
		var textnode = document.createTextNode(user.name);
		td.appendChild(textnode);
		//var tmp = "<tr>";		
		//tmp += "<td>"+user.name+"</td>";
		for(j in user.answears){
			//tmp += "<td>"+user.answears[j]+"</td>";
			var td = document.createElement("td");
			tr.appendChild(td);
			var textnode = document.createTextNode(user.answears[j]);
			td.appendChild(textnode);
		}
		//tmp += "</tr>";
		//var textnode = document.createTextNode(tmp);
		//myTable.insertBefore(textnode, list.childNodes[0]);
		myTable.insertBefore(tr, myTable.lastChild);
	}	
}

function requestGameStatus() {
//window.onload = function(){
	//name = window.prompt("Please put your user name");
	var obj={
		"letter": "A",
		"time": 55,
		"scores": [
			{"name": "baton96", "score": 123},
			{"name": "emilia96", "score": 321}
		]
	}
	document.getElementById("letter").innerHTML = obj.letter;
	var names = "<table><tr><th>Name</th>";
	var scores = "<tr><th>Score</th>";
	for(i in obj.scores){
		score = obj.scores[i];
		names += "<td>"+score.name+"</td>";
		scores += "<td>"+score.score+"</td>";		
	}
	document.getElementById("scores").innerHTML = names+"</tr>"+scores+"</table>";
	timer(obj.time);
}

function sendName() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4){
		if(this.status == 200) {
			window.alert("You can start playing");
		}
		else {
			window.alert("Something went wrong while picking user name");
		}
	}
      //document.getElementById("demo").innerHTML = this.responseText;
	  //var obj = JSON.parse(this.responseText);
	  //document.getElementById("demo").innerHTML = obj.login;
  };
  xhttp.open("POST", "http://192.168.10.2:1234", true); 
  xhttp.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
  xhttp.send("name="+name);
}

function requestGame() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4){
		if(this.status == 200) {
			var obj = JSON.parse(this.responseText);
		}
		else {
			window.alert("Something went wrong while requesting game status");
		}
	}
  };
  xhttp.open("POST", "http://192.168.10.2:1234", true); 
  xhttp.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
  xhttp.send("name="+name);
}

