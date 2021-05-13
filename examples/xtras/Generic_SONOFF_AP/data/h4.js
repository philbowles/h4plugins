let source
let colortable=["red","orange","yellow","green","blue","bi"]
const RECORD_SEPARATOR="|"
const UNIT_SEPARATOR="~"

function toaster(t){
    msg.innerHTML=t;
    setTimeout(function () { msg.innerHTML="&nbsp;"; },30000);
}

function ajax(url,decode=true){
    document.body.style.cursor = "wait";
    toaster("&nbsp;");
    var http = new XMLHttpRequest();
    var fullurl="http://"+window.location.hostname+"/rest/"+url;
    http.open('GET', fullurl);
    let r=document.getElementById("reply")
    http.addEventListener("load", function(e){
        document.body.style.cursor = "default";
        if(decode){
            r.innerHTML=""
            r.style.color="#ffffff" // css
            var j=JSON.parse(e.currentTarget.responseText);
            if(j.res) toaster("Error: "+j.res+" "+j.msg)
            else {
                j.lines.forEach(function(l){ r.innerHTML+=l+'\n'})
                document.querySelectorAll("input.edit").forEach(x => x.classList.remove("edit") )
            }
        }
    })
    http.send();
}

function properCase(s){ return s.split(" ").map(function (x){ return x.charAt(0).toUpperCase() + x.slice(1) }).join(" ") }

function makeTitle(i,cl){
    let e=document.createElement("div");
    e.classList="title "+cl;
    e.innerHTML=properCase(i)
    return e;
}

function makeBaseValueNode(id,type,cl,f){
    let n=document.createElement(type);
    n.id=id;
    if(cl.length) n.classList=cl;
    source.addEventListener(id, (e) => f(document.getElementById(id),e) )
    return n;
}

function ajaxvalue(e){ ajax("h4/config/"+e.target.id+"=`"+e.target.value+"`") }

function btnImg(n,v){ n.src="/"+n.id+parseInt(v)+".jpg" }

function options(elem,value){
    elem.innerHTML="";
    let opts=value.split(RECORD_SEPARATOR)
    opts.sort()
    opts.forEach(function(e){
        let kv=e.split(UNIT_SEPARATOR);
        let opt=document.createElement("option");
        opt.value=kv[1];
        opt.innerHTML=kv[0];
        elem.appendChild(opt);
    })
}

function makeValue(id,type,active,color,value){
    let valueNode;
    let imgNode;
    switch(type){
        case 0: // TEXT
            valueNode=makeBaseValueNode(id,"div","uv",(n,e) => n.innerHTML=e.data )
            valueNode.innerHTML=value;
            break;
        case 1: // GPIO
        case 2: // BOOL
            let hue=colortable[color];
            valueNode=makeBaseValueNode(id,"div","ld led-"+hue+"-"+parseInt(value),(n,e) => {
                let b=parseInt(e.data)
                let C="led-"+hue+"-";
                n.classList.remove(C+(b ? 0:1));
                n.classList.add(C+b);
            })
            break;
        case 3: // input
            valueNode=makeBaseValueNode(id,"input","",(n,e) => { n.value=e.data })
            valueNode.type="text";
            valueNode.value=value;
            valueNode.addEventListener("focus", (e) => e.target.classList.add("edit") )
            valueNode.addEventListener("blur",  (e) => {
                let n=document.getElementById(e.target.id);
                if(n.value!=value) ajaxvalue(e)
                else n.classList.remove("edit")
            })
            break;
        case 4: // image button
            imgnode=makeBaseValueNode(id,"input","",(n,e) => btnImg(n,e.data) )
            imgnode.type="image";
            btnImg(imgnode,value);
            imgnode.addEventListener("click", (e) => {
                let n=document.getElementById(e.target.id);
                let v=parseInt(n.src.replace(".jpg","").slice(-1))
                ajax("h4/config/"+n.id+"="+(v ? 0:1))
            })
            valueNode=document.createElement("div")
            valueNode.classList="bouter"
            let binner=document.createElement("div")
            binner.classList="binner"
            binner.appendChild(imgnode)
            valueNode.appendChild(binner)
            break;
        case 5: // dropdown
            valueNode=makeBaseValueNode(id,"select","",(n,e) => options(n,e.data) )
            options(valueNode,value)
            valueNode.addEventListener("change", (e) => ajaxvalue(e) )
            break;
        case 6: // image
            valueNode=document.createElement("div")
            valueNode.classList="bouter"
            imgnode=document.createElement("img");
            imgnode.id=id;
            imgnode.src=value;
            valueNode.appendChild(imgnode)
            source.addEventListener(id, (e) => document.getElementById(id).src=e.data )
            break;
        default:
            valueNode=makeBaseValueNode(id,"div","uv");
            valueNode.innerHTML="NYI "+value;
    }
    return valueNode;
}

function ui2(d){
    let parts=d.split(",")
    let id=parts[0]
    if(!document.getElementById(id)){
        let type=parseInt(parts[1])
        let hanger=document.getElementById(parts[2]+"hang")
        hanger.style="display: grid" // light 'em up (unused stays invisible)
        let active=parseInt(parts[3])
        let color=parts[4]
        let value=parts[5]
        if(type==4 || type==6) hanger.insertBefore(makeValue(id,type,active,color,value),null) // img button = full-width
        else {
            hanger.insertBefore(makeTitle(id,active ? "tuia":""),null)
            hanger.insertBefore(makeValue(id,type,active,color,value),null)
        }
    }
}

document.addEventListener("DOMContentLoaded", function() {
    if(!!window.EventSource) source=new EventSource("/evt");
    let rr=document.getElementById("reply")
    let cmd=document.getElementById("cmd")
    let msg=document.getElementById("msg")

    source.addEventListener('ui', function(e){ ui2(e.data) });
    source.onmessage=function(e){ toaster(e.data) }
    source.onerror=function(e){ document.querySelectorAll('div.hanger').forEach((d) => d.style.background="#ff0000") }

    toaster("Thank you for using H4/Plugins - please support me on Patreon, see link below");
        
    document.getElementById("cc").addEventListener('submit', function(e){
        e.preventDefault();
        ajax(cmd.value,true);
    },{capture: true});

    rr.addEventListener('click', function(e){
        let lines=rr.value.split("\n")
        let n=rr.value.substr(0, rr.selectionStart).split("\n").length
        cmd.value=lines[n-1]
        ajax(lines[n-1],true)
    },{capture: true});

    document.getElementById("qh").addEventListener('click', function(e){ ajax("help",true) },{capture: true});
})