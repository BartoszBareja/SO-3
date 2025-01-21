Import-Module ImportExcel

$inputFile = "adresy.xlsx"
$outputFile = "wyniki.xlsx"

$ipAddresses = Import-Excel -Path $inputFile -WorksheetName "IP-Addresses" | Select-Object -First 5

$results = @()

foreach ($ip in $ipAddresses) {
    $address = $ip.'Adres'

    if (-not [string]::IsNullOrEmpty($address)) {
        $result = Test-Connection -ComputerName $address -Count 1 -ErrorAction SilentlyContinue

        if ($result) {
            $output = "Dostępny"
        } else {
            $output = "Niedostępny"
        }

        $results += [PSCustomObject]@{
            Adres = $address
            Wynik = $output
        }
    } else {
        $results += [PSCustomObject]@{
            Adres = $address
            Wynik = "Brak adresu"
        }
    }
}

$results | Export-Excel -Path $outputFile -WorksheetName "Wyniki" -AutoSize
