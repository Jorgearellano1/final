import React, { useState } from 'react';
import './App.css';

const App = () => {
    const [key, setKey] = useState('');
    const [value, setValue] = useState('');
    const [message, setMessage] = useState('');
    const [table1, setTable1] = useState([]);
    const [table2, setTable2] = useState([]);
    const [menuOption, setMenuOption] = useState('1');

    const handleKeyChange = (e) => {
        setKey(e.target.value);
    };

    const handleValueChange = (e) => {
        setValue(e.target.value);
    };

    const handleMenuChange = (e) => {
        setMenuOption(e.target.value);
        setMessage('');
        if (e.target.value !== '1') {
            setValue('');
        }
    };

    const handleOperation = async () => {
        let endpoint = '';
        let url = `http://localhost:8080`;

        switch (menuOption) {
            case '1':
                endpoint = `/insert?key=${encodeURIComponent(key)}&value=${encodeURIComponent(value)}`;
                break;
            case '2':
                endpoint = `/search?key=${encodeURIComponent(key)}`;
                break;
            case '3':
                endpoint = `/delete?key=${encodeURIComponent(key)}`;
                break;
            case '4':
                endpoint = `/print`;
                break;
            default:
                setMessage('Opción no válida');
                return;
        }

        url += endpoint;

        try {
            const response = await fetch(url);
            if (!response.ok) {
                throw new Error(`HTTP error! status: ${response.status}`);
            }
            const data = await response.text();

            if (menuOption === '4') {
                const tables = data.split("\n");
                setTable1(tables[0].trim().split(/\s+/));
                setTable2(tables[1].trim().split(/\s+/));
            } else {
                setMessage(data);
            }
        } catch (error) {
            console.error("Hubo un error:", error);
            setMessage(`Error al realizar la operación: ${error.message}`);
        }
    };

    const renderTable = (table, title) => (
        <div className="hash-table">
            <h2>{title}</h2>
            <table>
                <tbody>
                {table.map((item, index) => (
                    <tr key={index}>
                        <td>{item}</td>
                    </tr>
                ))}
                </tbody>
            </table>
        </div>
    );

    return (
        <div className="app-container">
            <h1>Cuckoo Hashing Demo</h1>
            <div className="controls-container">
                <select value={menuOption} onChange={handleMenuChange}>
                    <option value="1">Insertar un elemento</option>
                    <option value="2">Buscar un elemento</option>
                    <option value="3">Eliminar un elemento</option>
                    <option value="4">Imprimir la tabla</option>
                </select>
                <input type="text" value={key} onChange={handleKeyChange} placeholder="Clave" />
                {menuOption === '1' && (
                    <input type="text" value={value} onChange={handleValueChange} placeholder="Valor" />
                )}
                <button onClick={handleOperation}>Ejecutar</button>
            </div>
            <div className="response">
                <p>Respuesta:</p>
                <pre>{message}</pre> {/* Esto mantendrá los saltos de línea y espacios */}
            </div>
            {menuOption === '4' && (
                <div className="final-hash-tables">
                    <h3>Tablas hash finales:</h3>
                    <div className="tables-container">
                        {renderTable(table1, 'Hash Izquierda')}
                        {renderTable(table2, 'Hash Derecha')}
                    </div>
                </div>
            )}
        </div>
    );
};

export default App;
