import { getKindleValue } from "./requestHandler.js"

const inputbtn = document.getElementById("input-btn");
const inputtext = document.getElementById("input-text");
const kindlevalue = document.getElementById("kindle-value");

refreshKindleValue();

async function refreshKindleValue() {
  try {
    const text = await getKindleValue();
    kindlevalue.textContent = text;
  } catch (err) {
    console.error(err);
  }

}
