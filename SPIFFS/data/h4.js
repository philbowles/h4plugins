let onof

function ajax(url){
    var http = new XMLHttpRequest();
    var fullurl="http://"+window.location.hostname+"/rest/"+url;
    http.open('GET', fullurl);
    http.send();  
}
  
document.addEventListener("DOMContentLoaded", function() {
	let hasonof=parseInt(document.getElementById("hasonof").value);
    console.warn("has onof?",hasonof)
    if(hasonof){
        onof=document.getElementById("onof");
        onof.parentNode.style.display='block'
        onof.addEventListener('click', function(e){ 
;            ajax("h4/toggle");
        });
    } 
    if(!!window.EventSource){
        source=new EventSource("/evt");
        source.addEventListener('onof', function(e){ 
            console.warn("SSE ",e);
            onof.src=parseInt(e.data) ? "on.jpg":"of.jpg"
        });
    }
})