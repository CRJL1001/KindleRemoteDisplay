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
