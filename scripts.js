var myTable;
var newNode;

window.onload = function(){ showNewTable(); }

function isImage(fileName){
	var extension = fileName.slice(fileName.lastIndexOf(".")+1);
	var arr = ["png", "jpg", "webp", "gif", "bmp"];	
	if(arr.indexOf(extension) > -1) return true;
	else return false;
}

function showOldTable(){
	var content = document.getElementById("content");
	content.removeChild(content.childNodes[0]);
	content.appendChild(myTable);
}

function showEdit(fileName, dir){
	if(dir==null) dir = ".";
	var content = document.getElementById("content");
	content.removeChild(content.childNodes[0]);
	
	var edit = document.createElement("span");
	content.appendChild(edit);
	
	var cancelButton = document.createElement("button");
	cancelButton.onclick=function(){showOldTable()};
	var cancelText = document.createTextNode("CANCEL");
	cancelButton.appendChild(cancelText);
	edit.appendChild(cancelButton);
	
	var deleteButton = document.createElement("button");
	deleteButton.onclick=function(){
		if(confirm("Are you sure you want to delete "+fileName+"?")){
			var xhttp2 = new XMLHttpRequest();
			xhttp2.onreadystatechange = function() {
				if (this.readyState == 4){
					if(this.status==200){
						showNewTable();
						alert("You have successfully deleted "+fileName);
					}
					else {
						showOldTable();
						if(this.status==403) alert("You dont have permission to delete "+fileName);
						else if(this.status==404) alert("Theres no such file as "+fileName);
						else {
							alert("Something went wrong so "+fileName+" couldnt be deleted");
							console.log(this.status+" "+this.statusText);
						}
					}
				}
			}
			xhttp2.open("DELETE", "http://"+location.host+"/"+dir+"/"+fileName, true); 
			xhttp2.send();
		}
	}
	var deleteText = document.createTextNode("DELETE");
	deleteButton.appendChild(deleteText);
	edit.appendChild(deleteButton);
	
	if(!isImage(fileName)){
		var xhttp = new XMLHttpRequest();
		xhttp.onreadystatechange = function() {
			if (this.readyState == 4){
				if(this.status==200){			
					var textarea = document.createElement("textarea");
					textarea.onkeydown = function(e) {
						if(e.keyCode==9 || e.which==9){
							e.preventDefault();
							var s = this.selectionStart;
							var e = this.selectionEnd;
							this.value = (this.value.substring(0, s)+"\t"+this.value.substring(e));
							this.selectionStart = this.selectionEnd = s+1;
						}
					}
					var textnode = document.createTextNode(this.responseText);
					textarea.appendChild(textnode);				
				
					var saveButton = document.createElement("button");
					saveButton.onclick=function(){
						if(confirm("Are you sure you want to overwrite "+fileName+"?")){
							var xhttp2 = new XMLHttpRequest();
							xhttp2.onreadystatechange = function() {
								if (this.readyState == 4){
									if(this.status==200){
										showNewTable();
										alert("You have successfully overwritten "+fileName);
									}
									else{
										showOldTable();
										if(this.status==403) alert("You dont have permission to overwrite "+fileName);
										else if(this.status==404) alert("Theres no such file as "+fileName);
										else{
											alert("Something went wrong so "+fileName+" couldnt be overwritten");
											console.log(this.status+" "+this.statusText);
										}
									}
								}
							}
							xhttp2.open("PUT", "http://"+location.host+"/"+dir+"/"+fileName, true); 
							xhttp2.send(textarea.value);
						}
					}	
					var saveText = document.createTextNode("SAVE");
					saveButton.appendChild(saveText);
					
					edit.appendChild(saveButton);
					edit.appendChild(document.createElement("br"));
					edit.appendChild(textarea);
				}
				else{
					showOldTable();
					if(this.status==403) alert("You dont have access to "+fileName);
					else if(this.status==404) alert("Theres no such file as "+fileName);
					else {
						alert("Something went wrong so "+fileName+" couldnt be accessed");
						console.log(this.status+" "+this.statusText);
					}
				}
			}
		}
		xhttp.open("GET", "http://"+location.host+"/"+dir+"/"+fileName, true); 
		xhttp.send();
	}
	else {
		var image = document.createElement("img");
		edit.appendChild(document.createElement("br"));
		edit.appendChild(image);
		image.src = (dir+"/"+fileName);
	}	
}

function showNew(dir, addDir){
	if(dir==null) dir = ".";
	if(addDir==null) addDir = false;
	var content = document.getElementById("content");
	content.removeChild(content.childNodes[0]);		
	newNode = document.createElement("span");
	content.appendChild(newNode);
	
	var textarea = document.createElement("textarea");
	textarea.onkeydown = function(e) {
		if(e.keyCode==9 || e.which==9){
			e.preventDefault();
			var s = this.selectionStart;
			var e = this.selectionEnd;
			this.value = (this.value.substring(0, s)+"\t"+this.value.substring(e));
			this.selectionStart = this.selectionEnd = s+1;
		}
	}
	
	var cancelButton = document.createElement("button");
	cancelButton.onclick=function(){showOldTable()};
	var cancelText = document.createTextNode("CANCEL");
	cancelButton.appendChild(cancelText);
	
	var input = document.createElement("input");
		
	var saveButton = document.createElement("button");
	saveButton.onclick=function(){
		var fileName = input.value;
		var xhttp = new XMLHttpRequest();
		xhttp.onreadystatechange = function() {
			if (this.readyState == 4){
				if(this.status==200){
					if(confirm(fileName+" already exists.\nDo you want to overwrite it?")){
						var xhttp2 = new XMLHttpRequest();
						xhttp2.onreadystatechange = function() {
							if (this.readyState == 4){
								if(this.status==200){
									showNewTable();
									alert("You have successfully overwritten "+fileName);
								}
								else {
									showOldTable();
									if(this.status==403) alert("You dont have permission to overwrite "+fileName);
									else if(this.status==404) alert("Theres no such file as "+fileName);
									else {
										alert("Something went wrong so "+fileName+" couldnt be overwritten");
										console.log(this.status+" "+this.statusText);
									}
								}
							}
						}
						xhttp2.open("PUT", "http://"+location.host+"/"+dir+"/"+fileName, true); 
						xhttp2.send(textarea.value);
					}
				}
				else if(this.status==404){
					var xhttp2 = new XMLHttpRequest();
					xhttp2.onreadystatechange = function() {
						if (this.readyState == 4){
							if(this.status==201){
								showNewTable();
								alert("You have successfully created "+fileName);
							}
							else {
								showOldTable();
								if(this.status==403) alert("You dont have permission to create "+fileName);
								else if(this.status==404) alert("Theres no such file as "+fileName);
								else {
									alert("Something went wrong so "+fileName+" couldnt be created");
									console.log(this.status+" "+this.statusText);
								}
							}
						}
					}
					xhttp2.open("PUT", "http://"+location.host+"/"+dir+"/"+fileName, true);
					if(addDir){
						if(fileName.lastIndexOf(".")!=-1) alert("Directory name cant have extension");
						else xhttp2.send();
					}
					else{
						if(fileName.lastIndexOf(".")==-1) alert("File has to have extension");
						else xhttp2.send(textarea.value);
					}
				}
				else{
					showOldTable();
					if(this.status==403) alert("You dont have permission to access "+fileName);
					else {
						alert("Something went wrong so "+fileName+" couldnt be accessed");
						console.log(this.status+" "+this.statusText);
					}
				}
			}
		}
		xhttp.open("HEAD", "http://"+location.host+"/"+dir+"/"+fileName, true); 
		if(addDir){
			if(fileName.lastIndexOf(".")!=-1) alert("Directory name cant have extension");
			else xhttp.send();
		}
		else xhttp.send();
	}	
	var saveText = document.createTextNode("SAVE AS");
	saveButton.appendChild(saveText);
	
	newNode.appendChild(cancelButton);
	newNode.appendChild(saveButton);
	newNode.appendChild(input);
	newNode.appendChild(document.createElement("br"));
	if(!addDir) newNode.appendChild(textarea);
}

function showNewTable(dir){
	if(dir==null) dir = ".";
	var xhttp = new XMLHttpRequest();
	xhttp.onreadystatechange = function() {
		if (this.readyState == 4 && this.status==200){
			var myString = this.responseText;
			myString = myString.substr(myString.indexOf("\n")+1);
			var content = document.getElementById("content");
			content.removeChild(content.childNodes[0]);
			
			myTable = document.createElement("span");
			content.appendChild(myTable);
			
			if(dir!="."){
				var backButton = document.createElement("button");
				var crumb = dir.substr(0, dir.lastIndexOf("/"));
				backButton.onclick=function(){showNewTable(crumb)};
				var backText = document.createTextNode("BACK");
				backButton.appendChild(backText);
				myTable.appendChild(backButton);
			}
			
			var newButton = document.createElement("button");
			newButton.onclick=function(){showNew(dir)};
			var newText = document.createTextNode("CREATE FILE");
			newButton.appendChild(newText);
			myTable.appendChild(newButton);
			
			//<input type="button" value="SEND FILE" onclick="document.getElementById('file').click();" />
			//<input type='file' style="display:none;" id="file" onchange='openFile(event)'>
			
			
			
			var inputFile = document.createElement("input");
			inputFile.type = "file";
			inputFile.style = "display: none";
			inputFile.onchange = function(e){ 
				//console.log(event.target.files[0])
				var file = event.target.files[0];
				var xhttp = new XMLHttpRequest();
				xhttp.onreadystatechange = function() {
					if (this.readyState == 4){
						if(this.status==200){
							if(confirm("Theres already existing file named "+file.name+"\nDo you want to overwrite it?")){
								var xhttp2 = new XMLHttpRequest();
								xhttp2.onreadystatechange = function() {
									if (this.readyState == 4){
										if(this.status==200){
											showNewTable();
											alert("You have successfully overwritten "+file.name);
										}
										else {
											showOldTable();
											if(this.status==403) alert("You dont have permission to overwrite "+fileName);
											else if(this.status==404) alert("Theres no such file as "+fileName);
											else {
												alert("Something went wrong so "+fileName+" couldnt be overwritten");
												console.log(this.status+" "+this.statusText);
											}
										}
									}
								}
								xhttp2.open("PUT", "http://"+location.host+"/"+dir+"/"+file.name, true); 
								xhttp2.send(file);
							}
						}
						else if(this.status==404){
							var xhttp2 = new XMLHttpRequest();
							xhttp2.onreadystatechange = function() {
								if (this.readyState == 4){
									if(this.status==201){
										showNewTable();
										alert("You have successfully created "+file.name);
									}
									else {
										showOldTable();
										if(this.status==403) alert("You dont have permission to created "+fileName);
										else if(this.status==404) alert("Theres no such file as "+fileName);
										else {
											alert("Something went wrong so "+fileName+" couldnt be created");
											console.log(this.status+" "+this.statusText);
										}
									}
								}
							}
							xhttp2.open("PUT", "http://"+location.host+"/"+dir+"/"+file.name, true); 
							xhttp2.send(file);
						}
						else{
							showOldTable();
							if(this.status==403) alert("You dont have permission to access "+fileName);
							else {
								alert("Something went wrong so "+fileName+" couldnt be accessed");
								console.log(this.status+" "+this.statusText);
							}
						}
					}
				}
				xhttp.open("HEAD", "http://"+location.host+"/"+dir+"/"+file.name, true); 
				xhttp.send();
			}
			myTable.appendChild(inputFile);
			
			var inputButton = document.createElement("input");
			inputButton.type = "button";
			inputButton.value = "SEND FILE";
			inputButton.onclick = function(){ inputFile.click(); };
			myTable.appendChild(inputButton);
			
			var newDirButton = document.createElement("button");
			newDirButton.onclick=function(){showNew(dir, true)};
			var newText = document.createTextNode("CREATE DIRECTORY");
			newDirButton.appendChild(newText);
			myTable.appendChild(newDirButton);
			
			myTable2 = document.createElement("table");
			myTable.appendChild(myTable2);
			var lines = myString.split("\n");
			for(var i=0; i<lines.length-1; i++){
				var tr = document.createElement("tr");
				myTable2.appendChild(tr);
				var words = lines[i].split(/ +/g);
				for(j in words){
					var td = document.createElement("td");
					tr.appendChild(td);
					if(j==words.length-1){
						if(words[j].lastIndexOf(".")!=-1){
							var a = document.createElement("a");
							//a.href=("http://"+location.host+"/"+words[j]);
							a.onclick=function(){showEdit(this.innerText, dir)};
							td.appendChild(a);				
							var textnode = document.createTextNode(words[j]);
							a.appendChild(textnode);						
						
							var editButton = document.createElement("button");
							editButton.onclick=function(){showEdit(this.previousSibling.innerText, dir)};
							var editText = document.createTextNode("EDIT");
							editButton.appendChild(editText);
							tr.appendChild(editButton);							
						}
						else {
							var a = document.createElement("a");
							//a.href=("http://"+location.host+"/"+words[j]);
							a.onclick=function(){showNewTable(dir+"/"+this.innerText)};
							td.appendChild(a);				
							var textnode = document.createTextNode(words[j]);
							a.appendChild(textnode);
							
							var span = document.createElement("span");
							tr.appendChild(span);
						}
						
						var deleteButton = document.createElement("button");
						deleteButton.onclick=function(){
							var fileName = this.previousSibling.previousSibling.innerText;
							if(confirm("Are you sure you want to delete "+fileName+"?")){
								var xhttp2 = new XMLHttpRequest();
								xhttp2.onreadystatechange = function() {
									if (this.readyState == 4){
										if(this.status==200){
											showNewTable();
											alert("You have successfully deleted "+fileName);
										}
										else{
											showOldTable();
											if(this.status==403) alert("You dont have permission to delete "+fileName);
											else if(this.status==404) alert("Theres no such file as "+fileName);
											else {
												alert("Something went wrong so "+fileName+" couldnt be deleted");
												console.log(this.status+" "+this.statusText);
											}
										}
									}
								}
								xhttp2.open("DELETE", "http://"+location.host+"/"+dir+"/"+fileName, true); 
								xhttp2.send();
							}
						}
						var deleteText = document.createTextNode("DELETE");
						deleteButton.appendChild(deleteText);
						tr.appendChild(deleteButton);
			
					}else{
						var textnode = document.createTextNode(words[j]);
						td.appendChild(textnode);
					}
				}			
			}
		}
	};
	xhttp.open("GET", "http://"+location.host+"/"+dir, true); 
	xhttp.send();		
}