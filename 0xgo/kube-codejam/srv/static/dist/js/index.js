var FrontRequest = {

    "code_owner":'',
    "language":'',
    "version":'',
    "code":'',
    "input":''

}

async function SubmitRecord(){

    freq = JSON.parse(JSON.stringify(FrontRequest))

    freq["code_owner"] = $('#submit-email').val()

    freq["language"] = $('#submit-lang').val()

    freq["version"] = $('#submit-version').val()

    freq["code"] = $('#submit-code').val()

    freq["input"] = $('#submit-input').val()


    var res = await axios.post("/run-payload/register/google/oauth2", freq)

    console.log(res.data)


}



async function SendText(){

    var text_input = document.getElementById("text-input")

    var text_input_content = document.getElementById("text-input-content")

    text_input.classList.remove("show")

    var data = {"text":text_input_content.value}

    var res = await axios.post(SEND_TEXT_ADDR,data)

}


async function SendFile(){

    var chart_input =  document.getElementById("chart-input")

    chart_input.classList.remove("show")

    var form_data = new FormData();
    var input_files = document.getElementById('chart-input-file');
    form_data.append("file", input_files.files[0]);
    var res = await axios.post(SEND_FILE_ADDR, form_data, {
        headers: {
        'Content-Type': 'multipart/form-data'
        }
    })

    console.log(res.data)


}







