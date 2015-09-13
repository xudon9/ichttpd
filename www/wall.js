function gettime()
{
	var date=new Date();
	var hour=date.getHours();
    var minute=date.getMinutes();
    var second=date.getSeconds();
	minute=checkTime(minute);
    second=checkTime(second);
	var day=date.getDate();
	var month=date.getMonth()+1;
	var year=date.getFullYear();
	return (day+"/"+month+"/"+year+" "+hour+":"+minute+":"+second);
}

function checkTime(i)
{
if (i<10) 
  {i="0" + i}
  return i
}

function main()
{
	var tt=document.createElement("h3");
	var title=document.getElementById("title").value;
	var time=gettime();
	var titletime=document.createTextNode(title+"    "+time);
	tt.appendChild(titletime);
	var div=document.createElement("div");
	var content=document.createTextNode(document.getElementById("content").innerHTML);
	var p=document.createElement("p");
	p.appendChild(content);
	div.className="sheet";
	div.appendChild(tt);
	div.appendChild(p);
	var sheets=document.getElementById("sheets");
	sheets.insertBefore(div,sheets.childNodes[0]);
	

}


