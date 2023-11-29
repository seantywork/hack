var WS = {}

var READ_CHANNEL = {}

var FrontRequest = {

    "code_owner":'',
    "language":'',
    "version":'',
    "code":'',
    "input":''

}

var FrontResponse = {

    "status":'',
    "message":'',
    "resource":'',
    "stdout":'',
    "stderr":''

}



function RunWSInit(){

    var or_url = window.location.href

    var proto_url = or_url.split('://') 

    var proto = proto_url[0]

    var urlbody_trail = proto_url[1].split('/')

    var urlbody = urlbody_trail[0]

    var trail_path = '/run-payload-adm'

    if (proto == 'http'){

        proto = 'ws://'

        var urlbody_port = urlbody.split(':')

        urlbody = urlbody_port[0] + ':' + '15888'

    } else if (proto == 'https'){

        proto = 'wss://'
    }

    var ws_url = proto + urlbody + trail_path

    console.log(ws_url)

    if (!CODE_OWNER){
        alert("code owner is empty")
        return {}
    }

    var ws = new WebSocket(ws_url)

    ws.onopen = function (){

        freq = JSON.parse(JSON.stringify(FrontRequest))

        freq["code_owner"] = CODE_OWNER

        ws.send(JSON.stringify(freq))

    }

    ws.onmessage = function (evnt){

        var fresp = evnt.data

        READ_CHANNEL = JSON.parse(fresp)

        var status = READ_CHANNEL["status"]

        if (status == 'N'){

            HandleN(READ_CHANNEL["message"])


        } else if (status == 'Y'){

            HandleY(READ_CHANNEL["message"])

        } else if (status == 'BYE'){

            HandleBYE(READ_CHANNEL["message"], READ_CHANNEL["resource"], READ_CHANNEL["stdout"], READ_CHANNEL["stderr"])

        }


    }

    
    return ws

}


function HandleN(message){

    alert("Error: "+message)

    return
}

function HandleY(message){

    message += '\n'

    $('#app-progress-render').append(message)

    return

}

function HandleBYE(message, resource, stdout, stderr){


    message += '\n'

    $('#app-progress-render').append(message)

    var tmpl =
    `
    <tr>
        <th>RESULT</th>
    </tr>
    <tr>
        <td>RESOURCE: ${resource}</td>
    </tr>
    <tr>
        <td>STDOUT:  ${stdout}</td>
    </tr>
    <tr>
        <td>STDERR:  ${stderr}</td>
    </tr>

    `

    $('#app-result-render').html(tmpl)


    return

}



WS = RunWSInit()

console.log("running ws")








