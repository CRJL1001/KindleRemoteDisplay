import { getKindleValue, setKindleValue } from "./requestHandler.js"

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

inputbtn.addEventListener("click", async () => {
  try {
    const newValue = inputtext.value;
    await setKindleValue(newValue);
    console.log(`texte => ${newValue}`);
    inputtext.value = "";
    kindlevalue.textContent = newValue;
  } catch (err) {
    console.error(err);
  }
})
