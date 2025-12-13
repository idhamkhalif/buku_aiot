// Fungsi untuk menangani POST request
function doPost(e) {
  try {
    // Parse data JSON yang diterima dari ESP32
    var data = JSON.parse(e.postData.contents);
    
    // Buka spreadsheet yang aktif
    var sheet = SpreadsheetApp.getActiveSpreadsheet().getActiveSheet();
    
    // Ambil nilai dari data yang dikirimkan
    var lux = data.lux || '';
    var suara = data.suara || '';
    var temperature = data.temperature || '';
    var humidity = data.humidity || '';
    var co2 = data.co2 || '';
    var cuaca = data.cuaca || '';
    var panjang = data.panjang || '';
    var lebar = data.lebar || '';
    var datetime = data.datetime || '';
    
    // Menambahkan data ke spreadsheet
    sheet.appendRow([lux, suara, temperature, humidity, co2, cuaca, panjang, lebar, datetime]);
    
    // Kembalikan response sukses
    return ContentService.createTextOutput(
      JSON.stringify({status: 'success', message: 'Data berhasil disimpan'})
    ).setMimeType(ContentService.MimeType.JSON);
  } catch (error) {
    // Kembalikan response error jika terjadi kesalahan
    return ContentService.createTextOutput(
      JSON.stringify({status: 'error', message: error.message})
    ).setMimeType(ContentService.MimeType.JSON);
  }
}

// Fungsi opsional untuk menangani GET request (misalnya untuk testing)
function doGet(e) {
  return ContentService.createTextOutput('Web app is working!').setMimeType(ContentService.MimeType.TEXT);
}

