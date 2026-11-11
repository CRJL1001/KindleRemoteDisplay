export async function getKindleValue() {
  const requesteOptions = {
    method: "GET",
    redirect: "follow"
  };

  try {
    const response = await fetch("http://localhost:8080/message", requesteOptions);
    if (!response.ok)
      throw new Error(`HTTP ${response.status}`);
    return response.text();
  } catch (err) {
    console.error(err);
  }
  return undefined;
}

export async function setKindleValue(newtext) {
  const headers = new Headers();
  headers.append("Content-Type", "application/json");

  const json = JSON.stringify({
    "text": newtext
  });

  const requestOptions = {
    method: "POST",
    headers: headers,
    body: json,
    redirect: "follow"
  };

  try {
    const response = await fetch("http://localhost:8080/edit", requestOptions)
    if (!response.ok)
      throw new Error(`Erreur dans la requête : ${response.status}`);
    return response.text();
  } catch (err) {
    console.error(err);
  }
  return undefined
}
